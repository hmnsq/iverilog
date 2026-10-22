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

تهی waitable_hooks_s::run_waiting_threads_(vthread_t&threads)
{
	// Run the non-blocking event controls.
      last = &event_ctls;
      برای (evctl*cur = event_ctls; cur != 0;) {
	    اگر (cur->dec_and_run()) {
		  evctl*nxt = cur->next;
		  delete cur;
		  cur = nxt;
		  *last = cur;
	    } وگرنه {
		  last = &(cur->next);
		  cur = cur->next;
	    }
      }

      vthread_t tmp = threads;
      اگر (tmp == 0) بازگشت;
      threads = 0;

      vthread_schedule_list(tmp);
}

evctl::evctl(uint64_t ecount)
{
      ecount_ = ecount;
      next = 0;
}

منطقی evctl::dec_and_run()
{
      أكد(ecount_ != 0);

      ecount_ -= 1;
      اگر (ecount_ == 0) run_run();

      بازگشت ecount_ == 0;
}

evctl_real::evctl_real(__vpiHandle*handle, دوگانه value,
                       uint64_t ecount)
:evctl(ecount)
{
      handle_ = handle;
      مقدار_2 = value;
}

تهی evctl_real::run_run()
{
      t_vpi_مقدار val;

      val.format = vpiRealVal;
      val.value.real = مقدار_2;
      vpi_put_مقدار(handle_, &val, 0, vpiNoDelay);
}

تهی schedule_evctl(__vpiHandle*handle, دوگانه value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      أكد(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = جدید evctl_real(handle, value, ecount);
      ep->last = &((*(ep->last))->next);
}

evctl_vector::evctl_vector(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&value,
                           unsigned off, unsigned wid, uint64_t ecount)
:evctl(ecount), ptr_(ptr), مقدار_2(value)
{
      off_ = off;
      wid_ = wid;
}

تهی evctl_vector::run_run()
{
      اگر (wid_ != 0) {
	    vvp_send_vec4_pv(ptr_, مقدار_2, off_, wid_, 0);
      } وگرنه {
	    vvp_send_vec4(ptr_, مقدار_2, 0);
      }
}

تهی schedule_evctl(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&value,
                    unsigned offset, unsigned wid,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      أكد(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = جدید evctl_vector(ptr, value, offset, wid, ecount);
      ep->last = &((*(ep->last))->next);
}

evctl_array::evctl_array(vvp_array_t memory, unsigned index,
                         ثابت vvp_vector4_t&value, unsigned off,
                         uint64_t ecount)
:evctl(ecount), مقدار_2(value)
{
      mem_ = memory;
      idx_ = index;
      off_ = off;
}

تهی evctl_array::run_run()
{
      mem_->set_word(idx_, off_, مقدار_2);
}

تهی schedule_evctl(vvp_array_t memory, unsigned index,
                    ثابت vvp_vector4_t&value, unsigned offset,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      أكد(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = جدید evctl_array(memory, index, value, offset, ecount);
      ep->last = &((*(ep->last))->next);
}

evctl_array_r::evctl_array_r(vvp_array_t memory, unsigned index,
                             دوگانه value, uint64_t ecount)
:evctl(ecount)
{
      mem_ = memory;
      idx_ = index;
      مقدار_2 = value;
}

تهی evctl_array_r::run_run()
{
      mem_->set_word(idx_, مقدار_2);
}

تهی schedule_evctl(vvp_array_t memory, unsigned index,
                    دوگانه value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      أكد(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = جدید evctl_array_r(memory, index, value, ecount);
      ep->last = &((*(ep->last))->next);
}

درونخطی vvp_fun_edge::edge_t VVP_EDGE(vvp_bit4_t from, vvp_bit4_t to)
{
      بازگشت 1 << ((from << 2) | to);
}

ثابت vvp_fun_edge::edge_t vvp_edge_posedge
      = VVP_EDGE(BIT4_0,BIT4_1)
      | VVP_EDGE(BIT4_0,BIT4_X)
      | VVP_EDGE(BIT4_0,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_1)
      | VVP_EDGE(BIT4_Z,BIT4_1)
      ;

ثابت vvp_fun_edge::edge_t vvp_edge_negedge
      = VVP_EDGE(BIT4_1,BIT4_0)
      | VVP_EDGE(BIT4_1,BIT4_X)
      | VVP_EDGE(BIT4_1,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_0)
      | VVP_EDGE(BIT4_Z,BIT4_0)
      ;

ثابت vvp_fun_edge::edge_t vvp_edge_edge
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

ثابت vvp_fun_edge::edge_t vvp_edge_none    = 0;

ساختار vvp_fun_edge_state_s : عمومی waitable_state_s {
      vvp_fun_edge_state_s()
      {
            برای (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
                  bits[idx] = BIT4_X;
      }

      vvp_bit4_t bits[4];
};

vvp_fun_edge::vvp_fun_edge(edge_t e)
: edge_(e)
{
      برای (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
            bits_[idx] = BIT4_X;
}

vvp_fun_edge::~vvp_fun_edge()
{
}

منطقی vvp_fun_edge::recv_vec4_(ثابت vvp_vector4_t&bit,
                              vvp_bit4_t&old_bit, vthread_t&threads)
{
	/* See what kind of edge this represents. */
      edge_t mask = VVP_EDGE(old_bit, bit.value(0));

	/* Save the current input for the next time around. */
      old_bit = bit.value(0);

      اگر ((edge_ == vvp_edge_none) || (edge_ & mask)) {
	    run_waiting_threads_(threads);
            بازگشت درست;
      }
      بازگشت نادرست;
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

      بازگشت tmp;
}

تهی vvp_fun_edge_sa::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                                vvp_context_t)
{
      اگر (recv_vec4_(bit, bits_[port.port()], threads_)) {
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

تهی vvp_fun_edge_sa::recv_vec4_pv(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t)
{
      أكد(base == 0);
      اگر (recv_vec4_(bit, bits_[port.port()], threads_)) {
	    vvp_net_t*net = port.ptr();
	    net->send_vec4_pv(bit, base, vwid, 0);
      }
}

vvp_fun_edge_aa::vvp_fun_edge_aa(edge_t e)
: vvp_fun_edge(e)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(این, context_scope_);
}

vvp_fun_edge_aa::~vvp_fun_edge_aa()
{
}

تهی vvp_fun_edge_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, جدید vvp_fun_edge_state_s);
      reset_instance(context);
}

تهی vvp_fun_edge_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_edge_state_s*state = static_cast<vvp_fun_edge_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
      برای (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
            state->bits[idx] = bits_[idx];
}

#ifdef CHECK_WITH_VALGRIND
تهی vvp_fun_edge_aa::free_instance(vvp_context_t context)
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

      بازگشت tmp;
}

تهی vvp_fun_edge_aa::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                                vvp_context_t context)
{
      اگر (context) {
            vvp_fun_edge_state_s*state = static_cast<vvp_fun_edge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            اگر (recv_vec4_(bit, state->bits[port.port()], state->threads)) {
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(bit, context);
            }
      } وگرنه {
            context = context_scope_->live_contexts;
            وقتی (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            bits_[port.port()] = bit.value(0);
      }
}

رده anyedge_مقدار {

    عمومی:
      anyedge_مقدار() {};
      مجازی ~anyedge_مقدار() {};

      مجازی تهی reset() = 0;

      مجازی تهی duplicate(anyedge_مقدار*&dup) = 0;
};

رده anyedge_vec4_مقدار : عمومی anyedge_مقدار {

    عمومی:
      anyedge_vec4_مقدار() {};
      مجازی ~anyedge_vec4_مقدار() override {};

      تهی reset() override { old_bits.set_to_x(); }

      تهی set(ثابت vvp_vector4_t&bit) { old_bits = bit; };

      تهی duplicate(anyedge_مقدار*&dup) override;

      منطقی recv_vec4(ثابت vvp_vector4_t&bit);

      منطقی recv_vec4_pv(ثابت vvp_vector4_t&bit, unsigned base,
			unsigned vwid);

    خصوصی:
      vvp_vector4_t old_bits;
};

ایستا anyedge_vec4_مقدار*get_vec4_مقدار(anyedge_مقدار*&value)
{
      anyedge_vec4_مقدار*vec4_مقدار = dynamic_cast<anyedge_vec4_مقدار*>(value);
      اگر (!value) {
	    vec4_مقدار = جدید anyedge_vec4_مقدار();
	    delete value;
	    value = vec4_مقدار;
      }
      بازگشت vec4_مقدار;
}

رده anyedge_real_مقدار : عمومی anyedge_مقدار {

    عمومی:
      anyedge_real_مقدار() : old_bits(0.0) {};
      مجازی ~anyedge_real_مقدار() override {};

      تهی reset() override { old_bits = 0.0; }

      تهی set(دوگانه bit) { old_bits = bit; };

      تهی duplicate(anyedge_مقدار*&dup) override;

      منطقی recv_real(دوگانه bit);

    خصوصی:
      دوگانه old_bits;
};

ایستا anyedge_real_مقدار*get_real_مقدار(anyedge_مقدار*&value)
{
      anyedge_real_مقدار*real_مقدار = dynamic_cast<anyedge_real_مقدار*>(value);
      اگر (!value) {
	    real_مقدار = جدید anyedge_real_مقدار();
	    delete value;
	    value = real_مقدار;
      }
      بازگشت real_مقدار;
}

رده anyedge_string_مقدار : عمومی anyedge_مقدار {

    عمومی:
      anyedge_string_مقدار() {};
      مجازی ~anyedge_string_مقدار() override {};

      تهی reset() override { old_bits.clear(); }

      تهی set(ثابت std::string&bit) { old_bits = bit; };

      تهی duplicate(anyedge_مقدار*&dup) override;

      منطقی recv_string(ثابت std::string&bit);

    خصوصی:
      std::string old_bits;
};

ایستا anyedge_string_مقدار*get_string_مقدار(anyedge_مقدار*&value)
{
      anyedge_string_مقدار*string_مقدار = dynamic_cast<anyedge_string_مقدار*>(value);
      اگر (!value) {
	    string_مقدار = جدید anyedge_string_مقدار();
	    delete value;
	    value = string_مقدار;
      }
      بازگشت string_مقدار;
}

ساختار vvp_fun_anyedge_state_s : عمومی waitable_state_s {
      vvp_fun_anyedge_state_s()
      {
            برای (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
                  last_مقدار[idx] = 0;
      }

      ~vvp_fun_anyedge_state_s()
      {
            برای (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
                  delete last_مقدار[idx];
      }

      anyedge_مقدار *last_مقدار[4];
      vvp_fun_anyedge_state_s(ثابت vvp_fun_anyedge_state_s&) = delete;
      vvp_fun_anyedge_state_s& operator=(ثابت vvp_fun_anyedge_state_s&) = delete;
};

vvp_fun_anyedge::vvp_fun_anyedge()
{
      برای (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
	    last_مقدار[idx] = 0;
}

vvp_fun_anyedge::~vvp_fun_anyedge()
{
      برای (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
	    delete last_مقدار[idx];
}

تهی anyedge_vec4_مقدار::duplicate(anyedge_مقدار*&dup)
{
      anyedge_vec4_مقدار*dup_vec4 = get_vec4_مقدار(dup);
      أكد(dup_vec4);
      dup_vec4->set(old_bits);
}

منطقی anyedge_vec4_مقدار::recv_vec4(ثابت vvp_vector4_t&bit)
{
      منطقی flag = نادرست;

      اگر (old_bits.size() != bit.size()) {
	    اگر (old_bits.size() == 0) {
		    // Special case: If we've not seen any input yet
		    // (old_bits.size()==0) then replace it will a reference
		    // vector that is 'bx. Then compare that with the input
		    // to see if we are processing a change from 'bx.
		  old_bits = vvp_vector4_t(bit.size(), BIT4_X);
		  اگر (old_bits.eeq(bit))
			flag = نادرست;
		  وگرنه
			flag = درست;

	    } وگرنه {
		  flag = درست;
	    }

      } وگرنه {
	    برای (unsigned idx = 0 ;  idx < bit.size() ;  idx += 1) {
		  اگر (old_bits.value(idx) != bit.value(idx)) {
			flag = درست;
			بشکن;
		  }
	    }
      }

      اگر (flag) {
	    old_bits = bit;
      }

      بازگشت flag;
}

منطقی anyedge_vec4_مقدار::recv_vec4_pv(ثابت vvp_vector4_t&bit, unsigned base,
				      unsigned vwid)
{
      vvp_vector4_t tmp = old_bits;
      اگر (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid, BIT4_Z);
      أكد(base + bit.size()<= vwid);
      أكد(tmp.size() == vwid);
      tmp.set_vec(base, bit);

      بازگشت recv_vec4(tmp);
}

تهی anyedge_real_مقدار::duplicate(anyedge_مقدار*&dup)
{
      anyedge_real_مقدار*dup_real = get_real_مقدار(dup);
      أكد(dup_real);
      dup_real->set(old_bits);
}

منطقی anyedge_real_مقدار::recv_real(دوگانه bit)
{
      اگر (old_bits != bit) {
	    old_bits = bit;
            بازگشت درست;
      }
      بازگشت نادرست;
}

تهی anyedge_string_مقدار::duplicate(anyedge_مقدار*&dup)
{
      anyedge_string_مقدار*dup_string = get_string_مقدار(dup);
      أكد(dup_string);
      dup_string->set(old_bits);
}

منطقی anyedge_string_مقدار::recv_string(ثابت std::string&bit)
{
      اگر (old_bits != bit) {
	    old_bits = bit;
            بازگشت درست;
      }
      بازگشت نادرست;
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

      بازگشت tmp;
}

تهی vvp_fun_anyedge_sa::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                                   vvp_context_t)
{
      anyedge_vec4_مقدار*value = get_vec4_مقدار(last_مقدار[port.port()]);
      أكد(value);
      اگر (value->recv_vec4(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

تهی vvp_fun_anyedge_sa::recv_vec4_pv(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
				      unsigned base, unsigned vwid, vvp_context_t)
{
      anyedge_vec4_مقدار*value = get_vec4_مقدار(last_مقدار[port.port()]);
      أكد(value);
      اگر (value->recv_vec4_pv(bit, base, vwid)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

تهی vvp_fun_anyedge_sa::recv_real(vvp_net_ptr_t port, دوگانه bit,
                                   vvp_context_t)
{
      anyedge_real_مقدار*value = get_real_مقدار(last_مقدار[port.port()]);
      أكد(value);
      اگر (value->recv_real(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

تهی vvp_fun_anyedge_sa::recv_string(vvp_net_ptr_t port, ثابت std::string&bit,
				     vvp_context_t)
{
      anyedge_string_مقدار*value = get_string_مقدار(last_مقدار[port.port()]);
      أكد(value);
      اگر (value->recv_string(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

/*
 * An anyedge receiving an object should do nothing with it, but should
 * trigger waiting threads.
 */
تهی vvp_fun_anyedge_sa::recv_object(vvp_net_ptr_t port, vvp_object_t,
				     vvp_context_t)
{
      run_waiting_threads_(threads_);
      vvp_net_t*net = port.ptr();
      net->send_vec4(vvp_vector4_t(), 0);
}

vvp_fun_anyedge_aa::vvp_fun_anyedge_aa()
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(این, context_scope_);
}

vvp_fun_anyedge_aa::~vvp_fun_anyedge_aa()
{
}

تهی vvp_fun_anyedge_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, جدید vvp_fun_anyedge_state_s);
      reset_instance(context);
}

تهی vvp_fun_anyedge_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
      برای (unsigned idx = 0 ;  idx < 4 ;  idx += 1) {
	    اگر (last_مقدار[idx])
	          last_مقدار[idx]->duplicate(state->last_مقدار[idx]);
	    وگرنه اگر (state->last_مقدار[idx])
		  state->last_مقدار[idx]->reset();
      }
}

#ifdef CHECK_WITH_VALGRIND
تهی vvp_fun_anyedge_aa::free_instance(vvp_context_t context)
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

      بازگشت tmp;
}

تهی vvp_fun_anyedge_aa::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      اگر (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_vec4_مقدار*value = get_vec4_مقدار(state->last_مقدار[port.port()]);
            أكد(value);
            اگر (value->recv_vec4(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(bit, context);
            }
      } وگرنه {
            context = context_scope_->live_contexts;
            وقتی (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            anyedge_vec4_مقدار*value = get_vec4_مقدار(last_مقدار[port.port()]);
            أكد(value);
            value->set(bit);
      }
}

تهی vvp_fun_anyedge_aa::recv_real(vvp_net_ptr_t port, دوگانه bit,
                                   vvp_context_t context)
{
      اگر (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_real_مقدار*value = get_real_مقدار(state->last_مقدار[port.port()]);
            أكد(value);
            اگر (value->recv_real(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } وگرنه {
            context = context_scope_->live_contexts;
            وقتی (context) {
                  recv_real(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            anyedge_real_مقدار*value = get_real_مقدار(last_مقدار[port.port()]);
            أكد(value);
            value->set(bit);
      }
}

تهی vvp_fun_anyedge_aa::recv_string(vvp_net_ptr_t port, ثابت std::string&bit,
				     vvp_context_t context)
{
      اگر (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_string_مقدار*value = get_string_مقدار(state->last_مقدار[port.port()]);
            أكد(value);
            اگر (value->recv_string(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } وگرنه {
            context = context_scope_->live_contexts;
            وقتی (context) {
                  recv_string(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            anyedge_string_مقدار*value = get_string_مقدار(last_مقدار[port.port()]);
            أكد(value);
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

      بازگشت tmp;
}

تهی vvp_fun_event_or_sa::recv_vec4(vvp_net_ptr_t, ثابت vvp_vector4_t&bit,
                                    vvp_context_t)
{
      run_waiting_threads_(threads_);
      base_net_->send_vec4(bit, 0);
}

vvp_fun_event_or_aa::vvp_fun_event_or_aa(vvp_net_t*base_net)
: vvp_fun_event_or(base_net)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(این, context_scope_);
}

vvp_fun_event_or_aa::~vvp_fun_event_or_aa()
{
}

تهی vvp_fun_event_or_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, جدید waitable_state_s);
}

تهی vvp_fun_event_or_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
}

#ifdef CHECK_WITH_VALGRIND
تهی vvp_fun_event_or_aa::free_instance(vvp_context_t context)
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

      بازگشت tmp;
}

تهی vvp_fun_event_or_aa::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                                    vvp_context_t context)
{
      اگر (context) {
            waitable_state_s*state = static_cast<waitable_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            run_waiting_threads_(state->threads);
            base_net_->send_vec4(bit, context);
      } وگرنه {
            context = context_scope_->live_contexts;
            وقتی (context) {
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

      بازگشت tmp;
}

تهی vvp_named_event_sa::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                                   vvp_context_t)
{
      run_waiting_threads_(threads_);
      vvp_net_t*net = port.ptr();
      net->send_vec4(bit, 0);

      __vpiNamedEvent*obj = dynamic_cast<__vpiNamedEvent*>(handle_);
      أكد(obj);
      obj->run_vpi_callbacks();
}

vvp_named_event_aa::vvp_named_event_aa(__vpiHandle*h)
: vvp_named_event(h)
{
      context_idx_ = vpip_add_item_to_context(این, vpip_peek_context_scope());
}

vvp_named_event_aa::~vvp_named_event_aa()
{
}

تهی vvp_named_event_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, جدید waitable_state_s);
}

تهی vvp_named_event_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
}

#ifdef CHECK_WITH_VALGRIND
تهی vvp_named_event_aa::free_instance(vvp_context_t context)
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

      بازگشت tmp;
}

تهی vvp_named_event_aa::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      أكد(context);

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

ایستا تهی compile_event_or(نویسه*label, unsigned argc, ساختار symb_s*argv);

تهی compile_event(نویسه*label, نویسه*type, unsigned argc, ساختار symb_s*argv)
{
      vvp_net_fun_t*fun = 0;

      اگر (type == 0) {
	    compile_event_or(label, argc, argv);
	    بازگشت;
      }

      اگر (strcmp(type,"anyedge") == 0) {

	    آزاد(type);

            اگر (vpip_peek_current_scope()->is_automatic()) {
                  fun = جدید vvp_fun_anyedge_aa;
            } وگرنه {
                  fun = جدید vvp_fun_anyedge_sa;
            }

      } وگرنه {

	    vvp_fun_edge::edge_t edge_type = vvp_edge_none;

	    اگر (strcmp(type,"posedge") == 0)
		  edge_type = vvp_edge_posedge;
	    وگرنه اگر (strcmp(type,"negedge") == 0)
		  edge_type = vvp_edge_negedge;
	    وگرنه اگر (strcmp(type,"edge") == 0)
		  edge_type = vvp_edge_edge;

	    أكد(argc <= 4);
	    آزاد(type);

            اگر (vpip_peek_current_scope()->is_automatic()) {
                  fun = جدید vvp_fun_edge_aa(edge_type);
            } وگرنه {
                  fun = جدید vvp_fun_edge_sa(edge_type);
            }

      }

      vvp_net_t* ptr = جدید vvp_net_t;
      ptr->fun = fun;

      define_functor_نماد(label, ptr);
      آزاد(label);

      inputs_connect(ptr, argc, argv);
      آزاد(argv);
}

ایستا تهی compile_event_or(نویسه*label, unsigned argc, ساختار symb_s*argv)
{
      vvp_net_t*base_net = جدید vvp_net_t;
      اگر (vpip_peek_current_scope()->is_automatic()) {
            base_net->fun = جدید vvp_fun_event_or_aa(base_net);
      } وگرنه {
            base_net->fun = جدید vvp_fun_event_or_sa(base_net);
      }
      define_functor_نماد(label, base_net);
      آزاد(label);

	/* This is a simplified version of a wide functor. We don't
	   care about the data values or what port they arrived on,
	   so we can use a single shared functor. */
      vvp_net_t*curr_net = base_net;
      برای (unsigned idx = 0 ;  idx < argc ;  idx += 1) {
	    اگر (idx > 0 && (idx % 4) == 0) {
		  curr_net = جدید vvp_net_t;
		  curr_net->fun = base_net->fun;
	    }
	    درون_داد_connect(curr_net, idx % 4, argv[idx].text);
      }
      آزاد(argv);
}

/*
 * This handles the compile of named events. This functor has no
 * inputs, it is only accessed by behavioral trigger statements, which
 * in vvp are %set instructions.
 */
تهی compile_named_event(نویسه*label, نویسه*name, منطقی local_flag)
{
      vvp_net_t*ptr = جدید vvp_net_t;

      vpiHandle obj = vpip_make_named_event(name, ptr);

      اگر (vpip_peek_current_scope()->is_automatic()) {
            ptr->fun = جدید vvp_named_event_aa(obj);
      } وگرنه {
            ptr->fun = جدید vvp_named_event_sa(obj);
      }
      define_functor_نماد(label, ptr);
      compile_vpi_نماد(label, obj);
      اگر (! local_flag) vpip_attach_to_current_scope(obj);

      آزاد(label);
      delete[] name;
}

#ifdef CHECK_WITH_VALGRIND
تهی named_event_delete(__vpiHandle*handle)
{
      delete dynamic_cast<__vpiNamedEvent *>(handle);
}
#endif
