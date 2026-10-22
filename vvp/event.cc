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

ଶୂନ୍ୟ waitable_hooks_s::run_waiting_threads_(vthread_t&threads)
{
	// Run the non-blocking event controls.
      last = &event_ctls;
      ପାଇଁ (evctl*cur = event_ctls; cur != 0;) {
	    ଯଦି (cur->dec_and_run()) {
		  evctl*nxt = cur->next;
		  delete cur;
		  cur = nxt;
		  *last = cur;
	    } ଅନ୍ୟଥା {
		  last = &(cur->next);
		  cur = cur->next;
	    }
      }

      vthread_t tmp = threads;
      ଯଦି (tmp == 0) ଫେରନ୍ତୁ;
      threads = 0;

      vthread_schedule_list(tmp);
}

evctl::evctl(uint64_t ecount)
{
      ecount_ = ecount;
      next = 0;
}

ତାର୍କିକ evctl::dec_and_run()
{
      ପ୍ରତିଜ୍ଞା(ecount_ != 0);

      ecount_ -= 1;
      ଯଦି (ecount_ == 0) run_run();

      ଫେରନ୍ତୁ ecount_ == 0;
}

evctl_real::evctl_real(__vpiHandle*handle, ଦ୍ୱିଗୁଣ value,
                       uint64_t ecount)
:evctl(ecount)
{
      handle_ = handle;
      ମୂଲ୍ୟ_2 = value;
}

ଶୂନ୍ୟ evctl_real::run_run()
{
      t_vpi_ମୂଲ୍ୟ val;

      val.format = vpiRealVal;
      val.value.real = ମୂଲ୍ୟ_2;
      vpi_put_ମୂଲ୍ୟ(handle_, &val, 0, vpiNoDelay);
}

ଶୂନ୍ୟ schedule_evctl(__vpiHandle*handle, ଦ୍ୱିଗୁଣ value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      ପ୍ରତିଜ୍ଞା(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = ନୂତନ evctl_real(handle, value, ecount);
      ep->last = &((*(ep->last))->next);
}

evctl_vector::evctl_vector(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&value,
                           unsigned off, unsigned wid, uint64_t ecount)
:evctl(ecount), ptr_(ptr), ମୂଲ୍ୟ_2(value)
{
      off_ = off;
      wid_ = wid;
}

ଶୂନ୍ୟ evctl_vector::run_run()
{
      ଯଦି (wid_ != 0) {
	    vvp_send_vec4_pv(ptr_, ମୂଲ୍ୟ_2, off_, wid_, 0);
      } ଅନ୍ୟଥା {
	    vvp_send_vec4(ptr_, ମୂଲ୍ୟ_2, 0);
      }
}

ଶୂନ୍ୟ schedule_evctl(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&value,
                    unsigned offset, unsigned wid,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      ପ୍ରତିଜ୍ଞା(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = ନୂତନ evctl_vector(ptr, value, offset, wid, ecount);
      ep->last = &((*(ep->last))->next);
}

evctl_array::evctl_array(vvp_array_t memory, unsigned index,
                         ଅଚଳ vvp_vector4_t&value, unsigned off,
                         uint64_t ecount)
:evctl(ecount), ମୂଲ୍ୟ_2(value)
{
      mem_ = memory;
      idx_ = index;
      off_ = off;
}

ଶୂନ୍ୟ evctl_array::run_run()
{
      mem_->set_word(idx_, off_, ମୂଲ୍ୟ_2);
}

ଶୂନ୍ୟ schedule_evctl(vvp_array_t memory, unsigned index,
                    ଅଚଳ vvp_vector4_t&value, unsigned offset,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      ପ୍ରତିଜ୍ଞା(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = ନୂତନ evctl_array(memory, index, value, offset, ecount);
      ep->last = &((*(ep->last))->next);
}

evctl_array_r::evctl_array_r(vvp_array_t memory, unsigned index,
                             ଦ୍ୱିଗୁଣ value, uint64_t ecount)
:evctl(ecount)
{
      mem_ = memory;
      idx_ = index;
      ମୂଲ୍ୟ_2 = value;
}

ଶୂନ୍ୟ evctl_array_r::run_run()
{
      mem_->set_word(idx_, ମୂଲ୍ୟ_2);
}

ଶୂନ୍ୟ schedule_evctl(vvp_array_t memory, unsigned index,
                    ଦ୍ୱିଗୁଣ value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      ପ୍ରତିଜ୍ଞା(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = ନୂତନ evctl_array_r(memory, index, value, ecount);
      ep->last = &((*(ep->last))->next);
}

ଧାଡ଼ିଭିତରେ vvp_fun_edge::edge_t VVP_EDGE(vvp_bit4_t from, vvp_bit4_t to)
{
      ଫେରନ୍ତୁ 1 << ((from << 2) | to);
}

ଅଚଳ vvp_fun_edge::edge_t vvp_edge_posedge
      = VVP_EDGE(BIT4_0,BIT4_1)
      | VVP_EDGE(BIT4_0,BIT4_X)
      | VVP_EDGE(BIT4_0,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_1)
      | VVP_EDGE(BIT4_Z,BIT4_1)
      ;

ଅଚଳ vvp_fun_edge::edge_t vvp_edge_negedge
      = VVP_EDGE(BIT4_1,BIT4_0)
      | VVP_EDGE(BIT4_1,BIT4_X)
      | VVP_EDGE(BIT4_1,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_0)
      | VVP_EDGE(BIT4_Z,BIT4_0)
      ;

ଅଚଳ vvp_fun_edge::edge_t vvp_edge_edge
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

ଅଚଳ vvp_fun_edge::edge_t vvp_edge_none    = 0;

ସଂରଚନା vvp_fun_edge_state_s : ସର୍ବସାଧାରଣ waitable_state_s {
      vvp_fun_edge_state_s()
      {
            ପାଇଁ (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
                  bits[idx] = BIT4_X;
      }

      vvp_bit4_t bits[4];
};

vvp_fun_edge::vvp_fun_edge(edge_t e)
: edge_(e)
{
      ପାଇଁ (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
            bits_[idx] = BIT4_X;
}

vvp_fun_edge::~vvp_fun_edge()
{
}

ତାର୍କିକ vvp_fun_edge::recv_vec4_(ଅଚଳ vvp_vector4_t&bit,
                              vvp_bit4_t&old_bit, vthread_t&threads)
{
	/* See what kind of edge this represents. */
      edge_t mask = VVP_EDGE(old_bit, bit.value(0));

	/* Save the current input for the next time around. */
      old_bit = bit.value(0);

      ଯଦି ((edge_ == vvp_edge_none) || (edge_ & mask)) {
	    run_waiting_threads_(threads);
            ଫେରନ୍ତୁ ସତ୍ୟ;
      }
      ଫେରନ୍ତୁ ମିଥ୍ୟା;
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

      ଫେରନ୍ତୁ tmp;
}

ଶୂନ୍ୟ vvp_fun_edge_sa::recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                                vvp_context_t)
{
      ଯଦି (recv_vec4_(bit, bits_[port.port()], threads_)) {
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

ଶୂନ୍ୟ vvp_fun_edge_sa::recv_vec4_pv(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t)
{
      ପ୍ରତିଜ୍ଞା(base == 0);
      ଯଦି (recv_vec4_(bit, bits_[port.port()], threads_)) {
	    vvp_net_t*net = port.ptr();
	    net->send_vec4_pv(bit, base, vwid, 0);
      }
}

vvp_fun_edge_aa::vvp_fun_edge_aa(edge_t e)
: vvp_fun_edge(e)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(ଏହି, context_scope_);
}

vvp_fun_edge_aa::~vvp_fun_edge_aa()
{
}

ଶୂନ୍ୟ vvp_fun_edge_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, ନୂତନ vvp_fun_edge_state_s);
      reset_instance(context);
}

ଶୂନ୍ୟ vvp_fun_edge_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_edge_state_s*state = static_cast<vvp_fun_edge_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
      ପାଇଁ (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
            state->bits[idx] = bits_[idx];
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ vvp_fun_edge_aa::free_instance(vvp_context_t context)
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

      ଫେରନ୍ତୁ tmp;
}

ଶୂନ୍ୟ vvp_fun_edge_aa::recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                                vvp_context_t context)
{
      ଯଦି (context) {
            vvp_fun_edge_state_s*state = static_cast<vvp_fun_edge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            ଯଦି (recv_vec4_(bit, state->bits[port.port()], state->threads)) {
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(bit, context);
            }
      } ଅନ୍ୟଥା {
            context = context_scope_->live_contexts;
            ଯେତେବେଳେ (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            bits_[port.port()] = bit.value(0);
      }
}

ଶ୍ରେଣୀ anyedge_ମୂଲ୍ୟ {

    ସର୍ବସାଧାରଣ:
      anyedge_ମୂଲ୍ୟ() {};
      ଆଭାସୀ ~anyedge_ମୂଲ୍ୟ() {};

      ଆଭାସୀ ଶୂନ୍ୟ reset() = 0;

      ଆଭାସୀ ଶୂନ୍ୟ duplicate(anyedge_ମୂଲ୍ୟ*&dup) = 0;
};

ଶ୍ରେଣୀ anyedge_vec4_ମୂଲ୍ୟ : ସର୍ବସାଧାରଣ anyedge_ମୂଲ୍ୟ {

    ସର୍ବସାଧାରଣ:
      anyedge_vec4_ମୂଲ୍ୟ() {};
      ଆଭାସୀ ~anyedge_vec4_ମୂଲ୍ୟ() override {};

      ଶୂନ୍ୟ reset() override { old_bits.set_to_x(); }

      ଶୂନ୍ୟ set(ଅଚଳ vvp_vector4_t&bit) { old_bits = bit; };

      ଶୂନ୍ୟ duplicate(anyedge_ମୂଲ୍ୟ*&dup) override;

      ତାର୍କିକ recv_vec4(ଅଚଳ vvp_vector4_t&bit);

      ତାର୍କିକ recv_vec4_pv(ଅଚଳ vvp_vector4_t&bit, unsigned base,
			unsigned vwid);

    ବ୍ୟକ୍ତିଗତ:
      vvp_vector4_t old_bits;
};

ସ୍ଥିର anyedge_vec4_ମୂଲ୍ୟ*get_vec4_ମୂଲ୍ୟ(anyedge_ମୂଲ୍ୟ*&value)
{
      anyedge_vec4_ମୂଲ୍ୟ*vec4_ମୂଲ୍ୟ = dynamic_cast<anyedge_vec4_ମୂଲ୍ୟ*>(value);
      ଯଦି (!value) {
	    vec4_ମୂଲ୍ୟ = ନୂତନ anyedge_vec4_ମୂଲ୍ୟ();
	    delete value;
	    value = vec4_ମୂଲ୍ୟ;
      }
      ଫେରନ୍ତୁ vec4_ମୂଲ୍ୟ;
}

ଶ୍ରେଣୀ anyedge_real_ମୂଲ୍ୟ : ସର୍ବସାଧାରଣ anyedge_ମୂଲ୍ୟ {

    ସର୍ବସାଧାରଣ:
      anyedge_real_ମୂଲ୍ୟ() : old_bits(0.0) {};
      ଆଭାସୀ ~anyedge_real_ମୂଲ୍ୟ() override {};

      ଶୂନ୍ୟ reset() override { old_bits = 0.0; }

      ଶୂନ୍ୟ set(ଦ୍ୱିଗୁଣ bit) { old_bits = bit; };

      ଶୂନ୍ୟ duplicate(anyedge_ମୂଲ୍ୟ*&dup) override;

      ତାର୍କିକ recv_real(ଦ୍ୱିଗୁଣ bit);

    ବ୍ୟକ୍ତିଗତ:
      ଦ୍ୱିଗୁଣ old_bits;
};

ସ୍ଥିର anyedge_real_ମୂଲ୍ୟ*get_real_ମୂଲ୍ୟ(anyedge_ମୂଲ୍ୟ*&value)
{
      anyedge_real_ମୂଲ୍ୟ*real_ମୂଲ୍ୟ = dynamic_cast<anyedge_real_ମୂଲ୍ୟ*>(value);
      ଯଦି (!value) {
	    real_ମୂଲ୍ୟ = ନୂତନ anyedge_real_ମୂଲ୍ୟ();
	    delete value;
	    value = real_ମୂଲ୍ୟ;
      }
      ଫେରନ୍ତୁ real_ମୂଲ୍ୟ;
}

ଶ୍ରେଣୀ anyedge_string_ମୂଲ୍ୟ : ସର୍ବସାଧାରଣ anyedge_ମୂଲ୍ୟ {

    ସର୍ବସାଧାରଣ:
      anyedge_string_ମୂଲ୍ୟ() {};
      ଆଭାସୀ ~anyedge_string_ମୂଲ୍ୟ() override {};

      ଶୂନ୍ୟ reset() override { old_bits.clear(); }

      ଶୂନ୍ୟ set(ଅଚଳ std::string&bit) { old_bits = bit; };

      ଶୂନ୍ୟ duplicate(anyedge_ମୂଲ୍ୟ*&dup) override;

      ତାର୍କିକ recv_string(ଅଚଳ std::string&bit);

    ବ୍ୟକ୍ତିଗତ:
      std::string old_bits;
};

ସ୍ଥିର anyedge_string_ମୂଲ୍ୟ*get_string_ମୂଲ୍ୟ(anyedge_ମୂଲ୍ୟ*&value)
{
      anyedge_string_ମୂଲ୍ୟ*string_ମୂଲ୍ୟ = dynamic_cast<anyedge_string_ମୂଲ୍ୟ*>(value);
      ଯଦି (!value) {
	    string_ମୂଲ୍ୟ = ନୂତନ anyedge_string_ମୂଲ୍ୟ();
	    delete value;
	    value = string_ମୂଲ୍ୟ;
      }
      ଫେରନ୍ତୁ string_ମୂଲ୍ୟ;
}

ସଂରଚନା vvp_fun_anyedge_state_s : ସର୍ବସାଧାରଣ waitable_state_s {
      vvp_fun_anyedge_state_s()
      {
            ପାଇଁ (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
                  last_ମୂଲ୍ୟ[idx] = 0;
      }

      ~vvp_fun_anyedge_state_s()
      {
            ପାଇଁ (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
                  delete last_ମୂଲ୍ୟ[idx];
      }

      anyedge_ମୂଲ୍ୟ *last_ମୂଲ୍ୟ[4];
      vvp_fun_anyedge_state_s(ଅଚଳ vvp_fun_anyedge_state_s&) = delete;
      vvp_fun_anyedge_state_s& operator=(ଅଚଳ vvp_fun_anyedge_state_s&) = delete;
};

vvp_fun_anyedge::vvp_fun_anyedge()
{
      ପାଇଁ (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
	    last_ମୂଲ୍ୟ[idx] = 0;
}

vvp_fun_anyedge::~vvp_fun_anyedge()
{
      ପାଇଁ (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
	    delete last_ମୂଲ୍ୟ[idx];
}

ଶୂନ୍ୟ anyedge_vec4_ମୂଲ୍ୟ::duplicate(anyedge_ମୂଲ୍ୟ*&dup)
{
      anyedge_vec4_ମୂଲ୍ୟ*dup_vec4 = get_vec4_ମୂଲ୍ୟ(dup);
      ପ୍ରତିଜ୍ଞା(dup_vec4);
      dup_vec4->set(old_bits);
}

ତାର୍କିକ anyedge_vec4_ମୂଲ୍ୟ::recv_vec4(ଅଚଳ vvp_vector4_t&bit)
{
      ତାର୍କିକ flag = ମିଥ୍ୟା;

      ଯଦି (old_bits.size() != bit.size()) {
	    ଯଦି (old_bits.size() == 0) {
		    // Special case: If we've not seen any input yet
		    // (old_bits.size()==0) then replace it will a reference
		    // vector that is 'bx. Then compare that with the input
		    // to see if we are processing a change from 'bx.
		  old_bits = vvp_vector4_t(bit.size(), BIT4_X);
		  ଯଦି (old_bits.eeq(bit))
			flag = ମିଥ୍ୟା;
		  ଅନ୍ୟଥା
			flag = ସତ୍ୟ;

	    } ଅନ୍ୟଥା {
		  flag = ସତ୍ୟ;
	    }

      } ଅନ୍ୟଥା {
	    ପାଇଁ (unsigned idx = 0 ;  idx < bit.size() ;  idx += 1) {
		  ଯଦି (old_bits.value(idx) != bit.value(idx)) {
			flag = ସତ୍ୟ;
			ଭାଙ୍ଗ;
		  }
	    }
      }

      ଯଦି (flag) {
	    old_bits = bit;
      }

      ଫେରନ୍ତୁ flag;
}

ତାର୍କିକ anyedge_vec4_ମୂଲ୍ୟ::recv_vec4_pv(ଅଚଳ vvp_vector4_t&bit, unsigned base,
				      unsigned vwid)
{
      vvp_vector4_t tmp = old_bits;
      ଯଦି (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid, BIT4_Z);
      ପ୍ରତିଜ୍ଞା(base + bit.size()<= vwid);
      ପ୍ରତିଜ୍ଞା(tmp.size() == vwid);
      tmp.set_vec(base, bit);

      ଫେରନ୍ତୁ recv_vec4(tmp);
}

ଶୂନ୍ୟ anyedge_real_ମୂଲ୍ୟ::duplicate(anyedge_ମୂଲ୍ୟ*&dup)
{
      anyedge_real_ମୂଲ୍ୟ*dup_real = get_real_ମୂଲ୍ୟ(dup);
      ପ୍ରତିଜ୍ଞା(dup_real);
      dup_real->set(old_bits);
}

ତାର୍କିକ anyedge_real_ମୂଲ୍ୟ::recv_real(ଦ୍ୱିଗୁଣ bit)
{
      ଯଦି (old_bits != bit) {
	    old_bits = bit;
            ଫେରନ୍ତୁ ସତ୍ୟ;
      }
      ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ଶୂନ୍ୟ anyedge_string_ମୂଲ୍ୟ::duplicate(anyedge_ମୂଲ୍ୟ*&dup)
{
      anyedge_string_ମୂଲ୍ୟ*dup_string = get_string_ମୂଲ୍ୟ(dup);
      ପ୍ରତିଜ୍ଞା(dup_string);
      dup_string->set(old_bits);
}

ତାର୍କିକ anyedge_string_ମୂଲ୍ୟ::recv_string(ଅଚଳ std::string&bit)
{
      ଯଦି (old_bits != bit) {
	    old_bits = bit;
            ଫେରନ୍ତୁ ସତ୍ୟ;
      }
      ଫେରନ୍ତୁ ମିଥ୍ୟା;
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

      ଫେରନ୍ତୁ tmp;
}

ଶୂନ୍ୟ vvp_fun_anyedge_sa::recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                                   vvp_context_t)
{
      anyedge_vec4_ମୂଲ୍ୟ*value = get_vec4_ମୂଲ୍ୟ(last_ମୂଲ୍ୟ[port.port()]);
      ପ୍ରତିଜ୍ଞା(value);
      ଯଦି (value->recv_vec4(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

ଶୂନ୍ୟ vvp_fun_anyedge_sa::recv_vec4_pv(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
				      unsigned base, unsigned vwid, vvp_context_t)
{
      anyedge_vec4_ମୂଲ୍ୟ*value = get_vec4_ମୂଲ୍ୟ(last_ମୂଲ୍ୟ[port.port()]);
      ପ୍ରତିଜ୍ଞା(value);
      ଯଦି (value->recv_vec4_pv(bit, base, vwid)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

ଶୂନ୍ୟ vvp_fun_anyedge_sa::recv_real(vvp_net_ptr_t port, ଦ୍ୱିଗୁଣ bit,
                                   vvp_context_t)
{
      anyedge_real_ମୂଲ୍ୟ*value = get_real_ମୂଲ୍ୟ(last_ମୂଲ୍ୟ[port.port()]);
      ପ୍ରତିଜ୍ଞା(value);
      ଯଦି (value->recv_real(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

ଶୂନ୍ୟ vvp_fun_anyedge_sa::recv_string(vvp_net_ptr_t port, ଅଚଳ std::string&bit,
				     vvp_context_t)
{
      anyedge_string_ମୂଲ୍ୟ*value = get_string_ମୂଲ୍ୟ(last_ମୂଲ୍ୟ[port.port()]);
      ପ୍ରତିଜ୍ଞା(value);
      ଯଦି (value->recv_string(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

/*
 * An anyedge receiving an object should do nothing with it, but should
 * trigger waiting threads.
 */
ଶୂନ୍ୟ vvp_fun_anyedge_sa::recv_object(vvp_net_ptr_t port, vvp_object_t,
				     vvp_context_t)
{
      run_waiting_threads_(threads_);
      vvp_net_t*net = port.ptr();
      net->send_vec4(vvp_vector4_t(), 0);
}

vvp_fun_anyedge_aa::vvp_fun_anyedge_aa()
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(ଏହି, context_scope_);
}

vvp_fun_anyedge_aa::~vvp_fun_anyedge_aa()
{
}

ଶୂନ୍ୟ vvp_fun_anyedge_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, ନୂତନ vvp_fun_anyedge_state_s);
      reset_instance(context);
}

ଶୂନ୍ୟ vvp_fun_anyedge_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
      ପାଇଁ (unsigned idx = 0 ;  idx < 4 ;  idx += 1) {
	    ଯଦି (last_ମୂଲ୍ୟ[idx])
	          last_ମୂଲ୍ୟ[idx]->duplicate(state->last_ମୂଲ୍ୟ[idx]);
	    ଅନ୍ୟଥା ଯଦି (state->last_ମୂଲ୍ୟ[idx])
		  state->last_ମୂଲ୍ୟ[idx]->reset();
      }
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ vvp_fun_anyedge_aa::free_instance(vvp_context_t context)
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

      ଫେରନ୍ତୁ tmp;
}

ଶୂନ୍ୟ vvp_fun_anyedge_aa::recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      ଯଦି (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_vec4_ମୂଲ୍ୟ*value = get_vec4_ମୂଲ୍ୟ(state->last_ମୂଲ୍ୟ[port.port()]);
            ପ୍ରତିଜ୍ଞା(value);
            ଯଦି (value->recv_vec4(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(bit, context);
            }
      } ଅନ୍ୟଥା {
            context = context_scope_->live_contexts;
            ଯେତେବେଳେ (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            anyedge_vec4_ମୂଲ୍ୟ*value = get_vec4_ମୂଲ୍ୟ(last_ମୂଲ୍ୟ[port.port()]);
            ପ୍ରତିଜ୍ଞା(value);
            value->set(bit);
      }
}

ଶୂନ୍ୟ vvp_fun_anyedge_aa::recv_real(vvp_net_ptr_t port, ଦ୍ୱିଗୁଣ bit,
                                   vvp_context_t context)
{
      ଯଦି (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_real_ମୂଲ୍ୟ*value = get_real_ମୂଲ୍ୟ(state->last_ମୂଲ୍ୟ[port.port()]);
            ପ୍ରତିଜ୍ଞା(value);
            ଯଦି (value->recv_real(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } ଅନ୍ୟଥା {
            context = context_scope_->live_contexts;
            ଯେତେବେଳେ (context) {
                  recv_real(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            anyedge_real_ମୂଲ୍ୟ*value = get_real_ମୂଲ୍ୟ(last_ମୂଲ୍ୟ[port.port()]);
            ପ୍ରତିଜ୍ଞା(value);
            value->set(bit);
      }
}

ଶୂନ୍ୟ vvp_fun_anyedge_aa::recv_string(vvp_net_ptr_t port, ଅଚଳ std::string&bit,
				     vvp_context_t context)
{
      ଯଦି (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_string_ମୂଲ୍ୟ*value = get_string_ମୂଲ୍ୟ(state->last_ମୂଲ୍ୟ[port.port()]);
            ପ୍ରତିଜ୍ଞା(value);
            ଯଦି (value->recv_string(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } ଅନ୍ୟଥା {
            context = context_scope_->live_contexts;
            ଯେତେବେଳେ (context) {
                  recv_string(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            anyedge_string_ମୂଲ୍ୟ*value = get_string_ମୂଲ୍ୟ(last_ମୂଲ୍ୟ[port.port()]);
            ପ୍ରତିଜ୍ଞା(value);
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

      ଫେରନ୍ତୁ tmp;
}

ଶୂନ୍ୟ vvp_fun_event_or_sa::recv_vec4(vvp_net_ptr_t, ଅଚଳ vvp_vector4_t&bit,
                                    vvp_context_t)
{
      run_waiting_threads_(threads_);
      base_net_->send_vec4(bit, 0);
}

vvp_fun_event_or_aa::vvp_fun_event_or_aa(vvp_net_t*base_net)
: vvp_fun_event_or(base_net)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(ଏହି, context_scope_);
}

vvp_fun_event_or_aa::~vvp_fun_event_or_aa()
{
}

ଶୂନ୍ୟ vvp_fun_event_or_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, ନୂତନ waitable_state_s);
}

ଶୂନ୍ୟ vvp_fun_event_or_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ vvp_fun_event_or_aa::free_instance(vvp_context_t context)
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

      ଫେରନ୍ତୁ tmp;
}

ଶୂନ୍ୟ vvp_fun_event_or_aa::recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                                    vvp_context_t context)
{
      ଯଦି (context) {
            waitable_state_s*state = static_cast<waitable_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            run_waiting_threads_(state->threads);
            base_net_->send_vec4(bit, context);
      } ଅନ୍ୟଥା {
            context = context_scope_->live_contexts;
            ଯେତେବେଳେ (context) {
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

      ଫେରନ୍ତୁ tmp;
}

ଶୂନ୍ୟ vvp_named_event_sa::recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                                   vvp_context_t)
{
      run_waiting_threads_(threads_);
      vvp_net_t*net = port.ptr();
      net->send_vec4(bit, 0);

      __vpiNamedEvent*obj = dynamic_cast<__vpiNamedEvent*>(handle_);
      ପ୍ରତିଜ୍ଞା(obj);
      obj->run_vpi_callbacks();
}

vvp_named_event_aa::vvp_named_event_aa(__vpiHandle*h)
: vvp_named_event(h)
{
      context_idx_ = vpip_add_item_to_context(ଏହି, vpip_peek_context_scope());
}

vvp_named_event_aa::~vvp_named_event_aa()
{
}

ଶୂନ୍ୟ vvp_named_event_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, ନୂତନ waitable_state_s);
}

ଶୂନ୍ୟ vvp_named_event_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ vvp_named_event_aa::free_instance(vvp_context_t context)
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

      ଫେରନ୍ତୁ tmp;
}

ଶୂନ୍ୟ vvp_named_event_aa::recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      ପ୍ରତିଜ୍ଞା(context);

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

ସ୍ଥିର ଶୂନ୍ୟ compile_event_or(ଅକ୍ଷର*label, unsigned argc, ସଂରଚନା symb_s*argv);

ଶୂନ୍ୟ compile_event(ଅକ୍ଷର*label, ଅକ୍ଷର*type, unsigned argc, ସଂରଚନା symb_s*argv)
{
      vvp_net_fun_t*fun = 0;

      ଯଦି (type == 0) {
	    compile_event_or(label, argc, argv);
	    ଫେରନ୍ତୁ;
      }

      ଯଦି (strcmp(type,"anyedge") == 0) {

	    ମୁକ୍ତ(type);

            ଯଦି (vpip_peek_current_scope()->is_automatic()) {
                  fun = ନୂତନ vvp_fun_anyedge_aa;
            } ଅନ୍ୟଥା {
                  fun = ନୂତନ vvp_fun_anyedge_sa;
            }

      } ଅନ୍ୟଥା {

	    vvp_fun_edge::edge_t edge_type = vvp_edge_none;

	    ଯଦି (strcmp(type,"posedge") == 0)
		  edge_type = vvp_edge_posedge;
	    ଅନ୍ୟଥା ଯଦି (strcmp(type,"negedge") == 0)
		  edge_type = vvp_edge_negedge;
	    ଅନ୍ୟଥା ଯଦି (strcmp(type,"edge") == 0)
		  edge_type = vvp_edge_edge;

	    ପ୍ରତିଜ୍ଞା(argc <= 4);
	    ମୁକ୍ତ(type);

            ଯଦି (vpip_peek_current_scope()->is_automatic()) {
                  fun = ନୂତନ vvp_fun_edge_aa(edge_type);
            } ଅନ୍ୟଥା {
                  fun = ନୂତନ vvp_fun_edge_sa(edge_type);
            }

      }

      vvp_net_t* ptr = ନୂତନ vvp_net_t;
      ptr->fun = fun;

      define_functor_ପ୍ରତୀକ(label, ptr);
      ମୁକ୍ତ(label);

      inputs_connect(ptr, argc, argv);
      ମୁକ୍ତ(argv);
}

ସ୍ଥିର ଶୂନ୍ୟ compile_event_or(ଅକ୍ଷର*label, unsigned argc, ସଂରଚନା symb_s*argv)
{
      vvp_net_t*base_net = ନୂତନ vvp_net_t;
      ଯଦି (vpip_peek_current_scope()->is_automatic()) {
            base_net->fun = ନୂତନ vvp_fun_event_or_aa(base_net);
      } ଅନ୍ୟଥା {
            base_net->fun = ନୂତନ vvp_fun_event_or_sa(base_net);
      }
      define_functor_ପ୍ରତୀକ(label, base_net);
      ମୁକ୍ତ(label);

	/* This is a simplified version of a wide functor. We don't
	   care about the data values or what port they arrived on,
	   so we can use a single shared functor. */
      vvp_net_t*curr_net = base_net;
      ପାଇଁ (unsigned idx = 0 ;  idx < argc ;  idx += 1) {
	    ଯଦି (idx > 0 && (idx % 4) == 0) {
		  curr_net = ନୂତନ vvp_net_t;
		  curr_net->fun = base_net->fun;
	    }
	    ନିବେଶ_connect(curr_net, idx % 4, argv[idx].text);
      }
      ମୁକ୍ତ(argv);
}

/*
 * This handles the compile of named events. This functor has no
 * inputs, it is only accessed by behavioral trigger statements, which
 * in vvp are %set instructions.
 */
ଶୂନ୍ୟ compile_named_event(ଅକ୍ଷର*label, ଅକ୍ଷର*name, ତାର୍କିକ local_flag)
{
      vvp_net_t*ptr = ନୂତନ vvp_net_t;

      vpiHandle obj = vpip_make_named_event(name, ptr);

      ଯଦି (vpip_peek_current_scope()->is_automatic()) {
            ptr->fun = ନୂତନ vvp_named_event_aa(obj);
      } ଅନ୍ୟଥା {
            ptr->fun = ନୂତନ vvp_named_event_sa(obj);
      }
      define_functor_ପ୍ରତୀକ(label, ptr);
      compile_vpi_ପ୍ରତୀକ(label, obj);
      ଯଦି (! local_flag) vpip_attach_to_current_scope(obj);

      ମୁକ୍ତ(label);
      delete[] name;
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ named_event_delete(__vpiHandle*handle)
{
      delete dynamic_cast<__vpiNamedEvent *>(handle);
}
#endif
