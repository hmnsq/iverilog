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

শূন্য waitable_hooks_s::run_waiting_threads_(vthread_t&threads)
{
	// Run the non-blocking event controls.
      last = &event_ctls;
      বাবে (evctl*cur = event_ctls; cur != 0;) {
	    যদি (cur->dec_and_run()) {
		  evctl*nxt = cur->next;
		  delete cur;
		  cur = nxt;
		  *last = cur;
	    } অন্যথা {
		  last = &(cur->next);
		  cur = cur->next;
	    }
      }

      vthread_t tmp = threads;
      যদি (tmp == 0) ঘূৰাই;
      threads = 0;

      vthread_schedule_list(tmp);
}

evctl::evctl(uint64_t ecount)
{
      ecount_ = ecount;
      next = 0;
}

তার্কিক evctl::dec_and_run()
{
      প্রতিজ্ঞা(ecount_ != 0);

      ecount_ -= 1;
      যদি (ecount_ == 0) run_run();

      ঘূৰাই ecount_ == 0;
}

evctl_real::evctl_real(__vpiHandle*handle, দ্ৱিগুণ value,
                       uint64_t ecount)
:evctl(ecount)
{
      handle_ = handle;
      মান_2 = value;
}

শূন্য evctl_real::run_run()
{
      t_vpi_মান val;

      val.format = vpiRealVal;
      val.value.real = মান_2;
      vpi_put_মান(handle_, &val, 0, vpiNoDelay);
}

শূন্য schedule_evctl(__vpiHandle*handle, দ্ৱিগুণ value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      প্রতিজ্ঞা(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = নূতন evctl_real(handle, value, ecount);
      ep->last = &((*(ep->last))->next);
}

evctl_vector::evctl_vector(vvp_net_ptr_t ptr, ধ্রুৱ vvp_vector4_t&value,
                           unsigned off, unsigned wid, uint64_t ecount)
:evctl(ecount), ptr_(ptr), মান_2(value)
{
      off_ = off;
      wid_ = wid;
}

শূন্য evctl_vector::run_run()
{
      যদি (wid_ != 0) {
	    vvp_send_vec4_pv(ptr_, মান_2, off_, wid_, 0);
      } অন্যথা {
	    vvp_send_vec4(ptr_, মান_2, 0);
      }
}

শূন্য schedule_evctl(vvp_net_ptr_t ptr, ধ্রুৱ vvp_vector4_t&value,
                    unsigned offset, unsigned wid,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      প্রতিজ্ঞা(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = নূতন evctl_vector(ptr, value, offset, wid, ecount);
      ep->last = &((*(ep->last))->next);
}

evctl_array::evctl_array(vvp_array_t memory, unsigned index,
                         ধ্রুৱ vvp_vector4_t&value, unsigned off,
                         uint64_t ecount)
:evctl(ecount), মান_2(value)
{
      mem_ = memory;
      idx_ = index;
      off_ = off;
}

শূন্য evctl_array::run_run()
{
      mem_->set_word(idx_, off_, মান_2);
}

শূন্য schedule_evctl(vvp_array_t memory, unsigned index,
                    ধ্রুৱ vvp_vector4_t&value, unsigned offset,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      প্রতিজ্ঞা(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = নূতন evctl_array(memory, index, value, offset, ecount);
      ep->last = &((*(ep->last))->next);
}

evctl_array_r::evctl_array_r(vvp_array_t memory, unsigned index,
                             দ্ৱিগুণ value, uint64_t ecount)
:evctl(ecount)
{
      mem_ = memory;
      idx_ = index;
      মান_2 = value;
}

শূন্য evctl_array_r::run_run()
{
      mem_->set_word(idx_, মান_2);
}

শূন্য schedule_evctl(vvp_array_t memory, unsigned index,
                    দ্ৱিগুণ value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      প্রতিজ্ঞা(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = নূতন evctl_array_r(memory, index, value, ecount);
      ep->last = &((*(ep->last))->next);
}

অন্তঃস্থ vvp_fun_edge::edge_t VVP_EDGE(vvp_bit4_t from, vvp_bit4_t to)
{
      ঘূৰাই 1 << ((from << 2) | to);
}

ধ্রুৱ vvp_fun_edge::edge_t vvp_edge_posedge
      = VVP_EDGE(BIT4_0,BIT4_1)
      | VVP_EDGE(BIT4_0,BIT4_X)
      | VVP_EDGE(BIT4_0,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_1)
      | VVP_EDGE(BIT4_Z,BIT4_1)
      ;

ধ্রুৱ vvp_fun_edge::edge_t vvp_edge_negedge
      = VVP_EDGE(BIT4_1,BIT4_0)
      | VVP_EDGE(BIT4_1,BIT4_X)
      | VVP_EDGE(BIT4_1,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_0)
      | VVP_EDGE(BIT4_Z,BIT4_0)
      ;

ধ্রুৱ vvp_fun_edge::edge_t vvp_edge_edge
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

ধ্রুৱ vvp_fun_edge::edge_t vvp_edge_none    = 0;

সংরচনা vvp_fun_edge_state_s : সার্ৱজনিক waitable_state_s {
      vvp_fun_edge_state_s()
      {
            বাবে (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
                  bits[idx] = BIT4_X;
      }

      vvp_bit4_t bits[4];
};

vvp_fun_edge::vvp_fun_edge(edge_t e)
: edge_(e)
{
      বাবে (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
            bits_[idx] = BIT4_X;
}

vvp_fun_edge::~vvp_fun_edge()
{
}

তার্কিক vvp_fun_edge::recv_vec4_(ধ্রুৱ vvp_vector4_t&bit,
                              vvp_bit4_t&old_bit, vthread_t&threads)
{
	/* See what kind of edge this represents. */
      edge_t mask = VVP_EDGE(old_bit, bit.value(0));

	/* Save the current input for the next time around. */
      old_bit = bit.value(0);

      যদি ((edge_ == vvp_edge_none) || (edge_ & mask)) {
	    run_waiting_threads_(threads);
            ঘূৰাই সঁচা;
      }
      ঘূৰাই মিছা;
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

      ঘূৰাই tmp;
}

শূন্য vvp_fun_edge_sa::recv_vec4(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
                                vvp_context_t)
{
      যদি (recv_vec4_(bit, bits_[port.port()], threads_)) {
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

শূন্য vvp_fun_edge_sa::recv_vec4_pv(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t)
{
      প্রতিজ্ঞা(base == 0);
      যদি (recv_vec4_(bit, bits_[port.port()], threads_)) {
	    vvp_net_t*net = port.ptr();
	    net->send_vec4_pv(bit, base, vwid, 0);
      }
}

vvp_fun_edge_aa::vvp_fun_edge_aa(edge_t e)
: vvp_fun_edge(e)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(এতত্, context_scope_);
}

vvp_fun_edge_aa::~vvp_fun_edge_aa()
{
}

শূন্য vvp_fun_edge_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, নূতন vvp_fun_edge_state_s);
      reset_instance(context);
}

শূন্য vvp_fun_edge_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_edge_state_s*state = static_cast<vvp_fun_edge_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
      বাবে (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
            state->bits[idx] = bits_[idx];
}

#ifdef CHECK_WITH_VALGRIND
শূন্য vvp_fun_edge_aa::free_instance(vvp_context_t context)
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

      ঘূৰাই tmp;
}

শূন্য vvp_fun_edge_aa::recv_vec4(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
                                vvp_context_t context)
{
      যদি (context) {
            vvp_fun_edge_state_s*state = static_cast<vvp_fun_edge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            যদি (recv_vec4_(bit, state->bits[port.port()], state->threads)) {
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(bit, context);
            }
      } অন্যথা {
            context = context_scope_->live_contexts;
            যেতিয়ালৈ (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            bits_[port.port()] = bit.value(0);
      }
}

শ্ৰেণী anyedge_মান {

    সার্ৱজনিক:
      anyedge_মান() {};
      আভাস ~anyedge_মান() {};

      আভাস শূন্য reset() = 0;

      আভাস শূন্য duplicate(anyedge_মান*&dup) = 0;
};

শ্ৰেণী anyedge_vec4_মান : সার্ৱজনিক anyedge_মান {

    সার্ৱজনিক:
      anyedge_vec4_মান() {};
      আভাস ~anyedge_vec4_মান() override {};

      শূন্য reset() override { old_bits.set_to_x(); }

      শূন্য set(ধ্রুৱ vvp_vector4_t&bit) { old_bits = bit; };

      শূন্য duplicate(anyedge_মান*&dup) override;

      তার্কিক recv_vec4(ধ্রুৱ vvp_vector4_t&bit);

      তার্কিক recv_vec4_pv(ধ্রুৱ vvp_vector4_t&bit, unsigned base,
			unsigned vwid);

    গুপ্ত:
      vvp_vector4_t old_bits;
};

স্থির anyedge_vec4_মান*get_vec4_মান(anyedge_মান*&value)
{
      anyedge_vec4_মান*vec4_মান = dynamic_cast<anyedge_vec4_মান*>(value);
      যদি (!value) {
	    vec4_মান = নূতন anyedge_vec4_মান();
	    delete value;
	    value = vec4_মান;
      }
      ঘূৰাই vec4_মান;
}

শ্ৰেণী anyedge_real_মান : সার্ৱজনিক anyedge_মান {

    সার্ৱজনিক:
      anyedge_real_মান() : old_bits(0.0) {};
      আভাস ~anyedge_real_মান() override {};

      শূন্য reset() override { old_bits = 0.0; }

      শূন্য set(দ্ৱিগুণ bit) { old_bits = bit; };

      শূন্য duplicate(anyedge_মান*&dup) override;

      তার্কিক recv_real(দ্ৱিগুণ bit);

    গুপ্ত:
      দ্ৱিগুণ old_bits;
};

স্থির anyedge_real_মান*get_real_মান(anyedge_মান*&value)
{
      anyedge_real_মান*real_মান = dynamic_cast<anyedge_real_মান*>(value);
      যদি (!value) {
	    real_মান = নূতন anyedge_real_মান();
	    delete value;
	    value = real_মান;
      }
      ঘূৰাই real_মান;
}

শ্ৰেণী anyedge_string_মান : সার্ৱজনিক anyedge_মান {

    সার্ৱজনিক:
      anyedge_string_মান() {};
      আভাস ~anyedge_string_মান() override {};

      শূন্য reset() override { old_bits.clear(); }

      শূন্য set(ধ্রুৱ std::string&bit) { old_bits = bit; };

      শূন্য duplicate(anyedge_মান*&dup) override;

      তার্কিক recv_string(ধ্রুৱ std::string&bit);

    গুপ্ত:
      std::string old_bits;
};

স্থির anyedge_string_মান*get_string_মান(anyedge_মান*&value)
{
      anyedge_string_মান*string_মান = dynamic_cast<anyedge_string_মান*>(value);
      যদি (!value) {
	    string_মান = নূতন anyedge_string_মান();
	    delete value;
	    value = string_মান;
      }
      ঘূৰাই string_মান;
}

সংরচনা vvp_fun_anyedge_state_s : সার্ৱজনিক waitable_state_s {
      vvp_fun_anyedge_state_s()
      {
            বাবে (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
                  last_মান[idx] = 0;
      }

      ~vvp_fun_anyedge_state_s()
      {
            বাবে (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
                  delete last_মান[idx];
      }

      anyedge_মান *last_মান[4];
      vvp_fun_anyedge_state_s(ধ্রুৱ vvp_fun_anyedge_state_s&) = delete;
      vvp_fun_anyedge_state_s& operator=(ধ্রুৱ vvp_fun_anyedge_state_s&) = delete;
};

vvp_fun_anyedge::vvp_fun_anyedge()
{
      বাবে (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
	    last_মান[idx] = 0;
}

vvp_fun_anyedge::~vvp_fun_anyedge()
{
      বাবে (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
	    delete last_মান[idx];
}

শূন্য anyedge_vec4_মান::duplicate(anyedge_মান*&dup)
{
      anyedge_vec4_মান*dup_vec4 = get_vec4_মান(dup);
      প্রতিজ্ঞা(dup_vec4);
      dup_vec4->set(old_bits);
}

তার্কিক anyedge_vec4_মান::recv_vec4(ধ্রুৱ vvp_vector4_t&bit)
{
      তার্কিক flag = মিছা;

      যদি (old_bits.size() != bit.size()) {
	    যদি (old_bits.size() == 0) {
		    // Special case: If we've not seen any input yet
		    // (old_bits.size()==0) then replace it will a reference
		    // vector that is 'bx. Then compare that with the input
		    // to see if we are processing a change from 'bx.
		  old_bits = vvp_vector4_t(bit.size(), BIT4_X);
		  যদি (old_bits.eeq(bit))
			flag = মিছা;
		  অন্যথা
			flag = সঁচা;

	    } অন্যথা {
		  flag = সঁচা;
	    }

      } অন্যথা {
	    বাবে (unsigned idx = 0 ;  idx < bit.size() ;  idx += 1) {
		  যদি (old_bits.value(idx) != bit.value(idx)) {
			flag = সঁচা;
			ভঙ্গ;
		  }
	    }
      }

      যদি (flag) {
	    old_bits = bit;
      }

      ঘূৰাই flag;
}

তার্কিক anyedge_vec4_মান::recv_vec4_pv(ধ্রুৱ vvp_vector4_t&bit, unsigned base,
				      unsigned vwid)
{
      vvp_vector4_t tmp = old_bits;
      যদি (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid, BIT4_Z);
      প্রতিজ্ঞা(base + bit.size()<= vwid);
      প্রতিজ্ঞা(tmp.size() == vwid);
      tmp.set_vec(base, bit);

      ঘূৰাই recv_vec4(tmp);
}

শূন্য anyedge_real_মান::duplicate(anyedge_মান*&dup)
{
      anyedge_real_মান*dup_real = get_real_মান(dup);
      প্রতিজ্ঞা(dup_real);
      dup_real->set(old_bits);
}

তার্কিক anyedge_real_মান::recv_real(দ্ৱিগুণ bit)
{
      যদি (old_bits != bit) {
	    old_bits = bit;
            ঘূৰাই সঁচা;
      }
      ঘূৰাই মিছা;
}

শূন্য anyedge_string_মান::duplicate(anyedge_মান*&dup)
{
      anyedge_string_মান*dup_string = get_string_মান(dup);
      প্রতিজ্ঞা(dup_string);
      dup_string->set(old_bits);
}

তার্কিক anyedge_string_মান::recv_string(ধ্রুৱ std::string&bit)
{
      যদি (old_bits != bit) {
	    old_bits = bit;
            ঘূৰাই সঁচা;
      }
      ঘূৰাই মিছা;
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

      ঘূৰাই tmp;
}

শূন্য vvp_fun_anyedge_sa::recv_vec4(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
                                   vvp_context_t)
{
      anyedge_vec4_মান*value = get_vec4_মান(last_মান[port.port()]);
      প্রতিজ্ঞা(value);
      যদি (value->recv_vec4(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

শূন্য vvp_fun_anyedge_sa::recv_vec4_pv(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
				      unsigned base, unsigned vwid, vvp_context_t)
{
      anyedge_vec4_মান*value = get_vec4_মান(last_মান[port.port()]);
      প্রতিজ্ঞা(value);
      যদি (value->recv_vec4_pv(bit, base, vwid)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

শূন্য vvp_fun_anyedge_sa::recv_real(vvp_net_ptr_t port, দ্ৱিগুণ bit,
                                   vvp_context_t)
{
      anyedge_real_মান*value = get_real_মান(last_মান[port.port()]);
      প্রতিজ্ঞা(value);
      যদি (value->recv_real(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

শূন্য vvp_fun_anyedge_sa::recv_string(vvp_net_ptr_t port, ধ্রুৱ std::string&bit,
				     vvp_context_t)
{
      anyedge_string_মান*value = get_string_মান(last_মান[port.port()]);
      প্রতিজ্ঞা(value);
      যদি (value->recv_string(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

/*
 * An anyedge receiving an object should do nothing with it, but should
 * trigger waiting threads.
 */
শূন্য vvp_fun_anyedge_sa::recv_object(vvp_net_ptr_t port, vvp_object_t,
				     vvp_context_t)
{
      run_waiting_threads_(threads_);
      vvp_net_t*net = port.ptr();
      net->send_vec4(vvp_vector4_t(), 0);
}

vvp_fun_anyedge_aa::vvp_fun_anyedge_aa()
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(এতত্, context_scope_);
}

vvp_fun_anyedge_aa::~vvp_fun_anyedge_aa()
{
}

শূন্য vvp_fun_anyedge_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, নূতন vvp_fun_anyedge_state_s);
      reset_instance(context);
}

শূন্য vvp_fun_anyedge_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
      বাবে (unsigned idx = 0 ;  idx < 4 ;  idx += 1) {
	    যদি (last_মান[idx])
	          last_মান[idx]->duplicate(state->last_মান[idx]);
	    অন্যথা যদি (state->last_মান[idx])
		  state->last_মান[idx]->reset();
      }
}

#ifdef CHECK_WITH_VALGRIND
শূন্য vvp_fun_anyedge_aa::free_instance(vvp_context_t context)
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

      ঘূৰাই tmp;
}

শূন্য vvp_fun_anyedge_aa::recv_vec4(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      যদি (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_vec4_মান*value = get_vec4_মান(state->last_মান[port.port()]);
            প্রতিজ্ঞা(value);
            যদি (value->recv_vec4(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(bit, context);
            }
      } অন্যথা {
            context = context_scope_->live_contexts;
            যেতিয়ালৈ (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            anyedge_vec4_মান*value = get_vec4_মান(last_মান[port.port()]);
            প্রতিজ্ঞা(value);
            value->set(bit);
      }
}

শূন্য vvp_fun_anyedge_aa::recv_real(vvp_net_ptr_t port, দ্ৱিগুণ bit,
                                   vvp_context_t context)
{
      যদি (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_real_মান*value = get_real_মান(state->last_মান[port.port()]);
            প্রতিজ্ঞা(value);
            যদি (value->recv_real(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } অন্যথা {
            context = context_scope_->live_contexts;
            যেতিয়ালৈ (context) {
                  recv_real(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            anyedge_real_মান*value = get_real_মান(last_মান[port.port()]);
            প্রতিজ্ঞা(value);
            value->set(bit);
      }
}

শূন্য vvp_fun_anyedge_aa::recv_string(vvp_net_ptr_t port, ধ্রুৱ std::string&bit,
				     vvp_context_t context)
{
      যদি (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_string_মান*value = get_string_মান(state->last_মান[port.port()]);
            প্রতিজ্ঞা(value);
            যদি (value->recv_string(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } অন্যথা {
            context = context_scope_->live_contexts;
            যেতিয়ালৈ (context) {
                  recv_string(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            anyedge_string_মান*value = get_string_মান(last_মান[port.port()]);
            প্রতিজ্ঞা(value);
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

      ঘূৰাই tmp;
}

শূন্য vvp_fun_event_or_sa::recv_vec4(vvp_net_ptr_t, ধ্রুৱ vvp_vector4_t&bit,
                                    vvp_context_t)
{
      run_waiting_threads_(threads_);
      base_net_->send_vec4(bit, 0);
}

vvp_fun_event_or_aa::vvp_fun_event_or_aa(vvp_net_t*base_net)
: vvp_fun_event_or(base_net)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(এতত্, context_scope_);
}

vvp_fun_event_or_aa::~vvp_fun_event_or_aa()
{
}

শূন্য vvp_fun_event_or_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, নূতন waitable_state_s);
}

শূন্য vvp_fun_event_or_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
}

#ifdef CHECK_WITH_VALGRIND
শূন্য vvp_fun_event_or_aa::free_instance(vvp_context_t context)
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

      ঘূৰাই tmp;
}

শূন্য vvp_fun_event_or_aa::recv_vec4(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
                                    vvp_context_t context)
{
      যদি (context) {
            waitable_state_s*state = static_cast<waitable_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            run_waiting_threads_(state->threads);
            base_net_->send_vec4(bit, context);
      } অন্যথা {
            context = context_scope_->live_contexts;
            যেতিয়ালৈ (context) {
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

      ঘূৰাই tmp;
}

শূন্য vvp_named_event_sa::recv_vec4(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
                                   vvp_context_t)
{
      run_waiting_threads_(threads_);
      vvp_net_t*net = port.ptr();
      net->send_vec4(bit, 0);

      __vpiNamedEvent*obj = dynamic_cast<__vpiNamedEvent*>(handle_);
      প্রতিজ্ঞা(obj);
      obj->run_vpi_callbacks();
}

vvp_named_event_aa::vvp_named_event_aa(__vpiHandle*h)
: vvp_named_event(h)
{
      context_idx_ = vpip_add_item_to_context(এতত্, vpip_peek_context_scope());
}

vvp_named_event_aa::~vvp_named_event_aa()
{
}

শূন্য vvp_named_event_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, নূতন waitable_state_s);
}

শূন্য vvp_named_event_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
}

#ifdef CHECK_WITH_VALGRIND
শূন্য vvp_named_event_aa::free_instance(vvp_context_t context)
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

      ঘূৰাই tmp;
}

শূন্য vvp_named_event_aa::recv_vec4(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      প্রতিজ্ঞা(context);

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

স্থির শূন্য compile_event_or(ৱর্ণ*label, unsigned argc, সংরচনা symb_s*argv);

শূন্য compile_event(ৱর্ণ*label, ৱর্ণ*type, unsigned argc, সংরচনা symb_s*argv)
{
      vvp_net_fun_t*fun = 0;

      যদি (type == 0) {
	    compile_event_or(label, argc, argv);
	    ঘূৰাই;
      }

      যদি (strcmp(type,"anyedge") == 0) {

	    মুক্ত(type);

            যদি (vpip_peek_current_scope()->is_automatic()) {
                  fun = নূতন vvp_fun_anyedge_aa;
            } অন্যথা {
                  fun = নূতন vvp_fun_anyedge_sa;
            }

      } অন্যথা {

	    vvp_fun_edge::edge_t edge_type = vvp_edge_none;

	    যদি (strcmp(type,"posedge") == 0)
		  edge_type = vvp_edge_posedge;
	    অন্যথা যদি (strcmp(type,"negedge") == 0)
		  edge_type = vvp_edge_negedge;
	    অন্যথা যদি (strcmp(type,"edge") == 0)
		  edge_type = vvp_edge_edge;

	    প্রতিজ্ঞা(argc <= 4);
	    মুক্ত(type);

            যদি (vpip_peek_current_scope()->is_automatic()) {
                  fun = নূতন vvp_fun_edge_aa(edge_type);
            } অন্যথা {
                  fun = নূতন vvp_fun_edge_sa(edge_type);
            }

      }

      vvp_net_t* ptr = নূতন vvp_net_t;
      ptr->fun = fun;

      define_functor_প্ৰতীক(label, ptr);
      মুক্ত(label);

      inputs_connect(ptr, argc, argv);
      মুক্ত(argv);
}

স্থির শূন্য compile_event_or(ৱর্ণ*label, unsigned argc, সংরচনা symb_s*argv)
{
      vvp_net_t*base_net = নূতন vvp_net_t;
      যদি (vpip_peek_current_scope()->is_automatic()) {
            base_net->fun = নূতন vvp_fun_event_or_aa(base_net);
      } অন্যথা {
            base_net->fun = নূতন vvp_fun_event_or_sa(base_net);
      }
      define_functor_প্ৰতীক(label, base_net);
      মুক্ত(label);

	/* This is a simplified version of a wide functor. We don't
	   care about the data values or what port they arrived on,
	   so we can use a single shared functor. */
      vvp_net_t*curr_net = base_net;
      বাবে (unsigned idx = 0 ;  idx < argc ;  idx += 1) {
	    যদি (idx > 0 && (idx % 4) == 0) {
		  curr_net = নূতন vvp_net_t;
		  curr_net->fun = base_net->fun;
	    }
	    প্ৰবেশ_connect(curr_net, idx % 4, argv[idx].text);
      }
      মুক্ত(argv);
}

/*
 * This handles the compile of named events. This functor has no
 * inputs, it is only accessed by behavioral trigger statements, which
 * in vvp are %set instructions.
 */
শূন্য compile_named_event(ৱর্ণ*label, ৱর্ণ*name, তার্কিক local_flag)
{
      vvp_net_t*ptr = নূতন vvp_net_t;

      vpiHandle obj = vpip_make_named_event(name, ptr);

      যদি (vpip_peek_current_scope()->is_automatic()) {
            ptr->fun = নূতন vvp_named_event_aa(obj);
      } অন্যথা {
            ptr->fun = নূতন vvp_named_event_sa(obj);
      }
      define_functor_প্ৰতীক(label, ptr);
      compile_vpi_প্ৰতীক(label, obj);
      যদি (! local_flag) vpip_attach_to_current_scope(obj);

      মুক্ত(label);
      delete[] name;
}

#ifdef CHECK_WITH_VALGRIND
শূন্য named_event_delete(__vpiHandle*handle)
{
      delete dynamic_cast<__vpiNamedEvent *>(handle);
}
#endif
