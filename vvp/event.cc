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

فراغ waitable_hooks_s::run_waiting_threads_(vthread_t&threads)
{
	// Run the non-blocking event controls.
      last = &event_ctls;
      עבור (evctl*cur = event_ctls; cur != 0;) {
	    אם (cur->dec_and_run()) {
		  evctl*nxt = cur->next;
		  delete cur;
		  cur = nxt;
		  *last = cur;
	    } אחרת {
		  last = &(cur->next);
		  cur = cur->next;
	    }
      }

      vthread_t tmp = threads;
      אם (tmp == 0) החזר;
      threads = 0;

      vthread_schedule_list(tmp);
}

evctl::evctl(uint64_t ecount)
{
      ecount_ = ecount;
      next = 0;
}

منطقي evctl::dec_and_run()
{
      أكد(ecount_ != 0);

      ecount_ -= 1;
      אם (ecount_ == 0) run_run();

      החזר ecount_ == 0;
}

evctl_real::evctl_real(__vpiHandle*handle, مزدوج value,
                       uint64_t ecount)
:evctl(ecount)
{
      handle_ = handle;
      value_ = value;
}

فراغ evctl_real::run_run()
{
      t_vpi_value val;

      val.format = vpiRealVal;
      val.value.real = value_;
      vpi_put_value(handle_, &val, 0, vpiNoDelay);
}

فراغ schedule_evctl(__vpiHandle*handle, مزدوج value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      أكد(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = جديد evctl_real(handle, value, ecount);
      ep->last = &((*(ep->last))->next);
}

evctl_vector::evctl_vector(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&value,
                           unsigned off, unsigned wid, uint64_t ecount)
:evctl(ecount), ptr_(ptr), value_(value)
{
      off_ = off;
      wid_ = wid;
}

فراغ evctl_vector::run_run()
{
      אם (wid_ != 0) {
	    vvp_send_vec4_pv(ptr_, value_, off_, wid_, 0);
      } אחרת {
	    vvp_send_vec4(ptr_, value_, 0);
      }
}

فراغ schedule_evctl(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&value,
                    unsigned offset, unsigned wid,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      أكد(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = جديد evctl_vector(ptr, value, offset, wid, ecount);
      ep->last = &((*(ep->last))->next);
}

evctl_array::evctl_array(vvp_array_t memory, unsigned index,
                         ثابت vvp_vector4_t&value, unsigned off,
                         uint64_t ecount)
:evctl(ecount), value_(value)
{
      mem_ = memory;
      idx_ = index;
      off_ = off;
}

فراغ evctl_array::run_run()
{
      mem_->set_word(idx_, off_, value_);
}

فراغ schedule_evctl(vvp_array_t memory, unsigned index,
                    ثابت vvp_vector4_t&value, unsigned offset,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      أكد(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = جديد evctl_array(memory, index, value, offset, ecount);
      ep->last = &((*(ep->last))->next);
}

evctl_array_r::evctl_array_r(vvp_array_t memory, unsigned index,
                             مزدوج value, uint64_t ecount)
:evctl(ecount)
{
      mem_ = memory;
      idx_ = index;
      value_ = value;
}

فراغ evctl_array_r::run_run()
{
      mem_->set_word(idx_, value_);
}

فراغ schedule_evctl(vvp_array_t memory, unsigned index,
                    مزدوج value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      أكد(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = جديد evctl_array_r(memory, index, value, ecount);
      ep->last = &((*(ep->last))->next);
}

مضمن vvp_fun_edge::edge_t VVP_EDGE(vvp_bit4_t from, vvp_bit4_t to)
{
      החזר 1 << ((from << 2) | to);
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

بنية vvp_fun_edge_state_s : عام waitable_state_s {
      vvp_fun_edge_state_s()
      {
            עבור (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
                  bits[idx] = BIT4_X;
      }

      vvp_bit4_t bits[4];
};

vvp_fun_edge::vvp_fun_edge(edge_t e)
: edge_(e)
{
      עבור (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
            bits_[idx] = BIT4_X;
}

vvp_fun_edge::~vvp_fun_edge()
{
}

منطقي vvp_fun_edge::recv_vec4_(ثابت vvp_vector4_t&bit,
                              vvp_bit4_t&old_bit, vthread_t&threads)
{
	/* See what kind of edge this represents. */
      edge_t mask = VVP_EDGE(old_bit, bit.value(0));

	/* Save the current input for the next time around. */
      old_bit = bit.value(0);

      אם ((edge_ == vvp_edge_none) || (edge_ & mask)) {
	    run_waiting_threads_(threads);
            החזר אמת;
      }
      החזר שקר;
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

      החזר tmp;
}

فراغ vvp_fun_edge_sa::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                                vvp_context_t)
{
      אם (recv_vec4_(bit, bits_[port.port()], threads_)) {
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

فراغ vvp_fun_edge_sa::recv_vec4_pv(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t)
{
      أكد(base == 0);
      אם (recv_vec4_(bit, bits_[port.port()], threads_)) {
	    vvp_net_t*net = port.ptr();
	    net->send_vec4_pv(bit, base, vwid, 0);
      }
}

vvp_fun_edge_aa::vvp_fun_edge_aa(edge_t e)
: vvp_fun_edge(e)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(هذا, context_scope_);
}

vvp_fun_edge_aa::~vvp_fun_edge_aa()
{
}

فراغ vvp_fun_edge_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, جديد vvp_fun_edge_state_s);
      reset_instance(context);
}

فراغ vvp_fun_edge_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_edge_state_s*state = static_cast<vvp_fun_edge_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
      עבור (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
            state->bits[idx] = bits_[idx];
}

#ifdef CHECK_WITH_VALGRIND
فراغ vvp_fun_edge_aa::free_instance(vvp_context_t context)
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

      החזר tmp;
}

فراغ vvp_fun_edge_aa::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                                vvp_context_t context)
{
      אם (context) {
            vvp_fun_edge_state_s*state = static_cast<vvp_fun_edge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            אם (recv_vec4_(bit, state->bits[port.port()], state->threads)) {
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(bit, context);
            }
      } אחרת {
            context = context_scope_->live_contexts;
            כלעוד (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            bits_[port.port()] = bit.value(0);
      }
}

מחלקה anyedge_value {

    عام:
      anyedge_value() {};
      virtual ~anyedge_value() {};

      virtual فراغ reset() = 0;

      virtual فراغ duplicate(anyedge_value*&dup) = 0;
};

מחלקה anyedge_vec4_value : عام anyedge_value {

    عام:
      anyedge_vec4_value() {};
      virtual ~anyedge_vec4_value() override {};

      فراغ reset() override { old_bits.set_to_x(); }

      فراغ set(ثابت vvp_vector4_t&bit) { old_bits = bit; };

      فراغ duplicate(anyedge_value*&dup) override;

      منطقي recv_vec4(ثابت vvp_vector4_t&bit);

      منطقي recv_vec4_pv(ثابت vvp_vector4_t&bit, unsigned base,
			unsigned vwid);

    خاص:
      vvp_vector4_t old_bits;
};

ساكن anyedge_vec4_value*get_vec4_value(anyedge_value*&value)
{
      anyedge_vec4_value*vec4_value = dynamic_cast<anyedge_vec4_value*>(value);
      אם (!value) {
	    vec4_value = جديد anyedge_vec4_value();
	    delete value;
	    value = vec4_value;
      }
      החזר vec4_value;
}

מחלקה anyedge_real_value : عام anyedge_value {

    عام:
      anyedge_real_value() : old_bits(0.0) {};
      virtual ~anyedge_real_value() override {};

      فراغ reset() override { old_bits = 0.0; }

      فراغ set(مزدوج bit) { old_bits = bit; };

      فراغ duplicate(anyedge_value*&dup) override;

      منطقي recv_real(مزدوج bit);

    خاص:
      مزدوج old_bits;
};

ساكن anyedge_real_value*get_real_value(anyedge_value*&value)
{
      anyedge_real_value*real_value = dynamic_cast<anyedge_real_value*>(value);
      אם (!value) {
	    real_value = جديد anyedge_real_value();
	    delete value;
	    value = real_value;
      }
      החזר real_value;
}

מחלקה anyedge_string_value : عام anyedge_value {

    عام:
      anyedge_string_value() {};
      virtual ~anyedge_string_value() override {};

      فراغ reset() override { old_bits.clear(); }

      فراغ set(ثابت std::string&bit) { old_bits = bit; };

      فراغ duplicate(anyedge_value*&dup) override;

      منطقي recv_string(ثابت std::string&bit);

    خاص:
      std::string old_bits;
};

ساكن anyedge_string_value*get_string_value(anyedge_value*&value)
{
      anyedge_string_value*string_value = dynamic_cast<anyedge_string_value*>(value);
      אם (!value) {
	    string_value = جديد anyedge_string_value();
	    delete value;
	    value = string_value;
      }
      החזר string_value;
}

بنية vvp_fun_anyedge_state_s : عام waitable_state_s {
      vvp_fun_anyedge_state_s()
      {
            עבור (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
                  last_value_[idx] = 0;
      }

      ~vvp_fun_anyedge_state_s()
      {
            עבור (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
                  delete last_value_[idx];
      }

      anyedge_value *last_value_[4];
      vvp_fun_anyedge_state_s(ثابت vvp_fun_anyedge_state_s&) = delete;
      vvp_fun_anyedge_state_s& operator=(ثابت vvp_fun_anyedge_state_s&) = delete;
};

vvp_fun_anyedge::vvp_fun_anyedge()
{
      עבור (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
	    last_value_[idx] = 0;
}

vvp_fun_anyedge::~vvp_fun_anyedge()
{
      עבור (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
	    delete last_value_[idx];
}

فراغ anyedge_vec4_value::duplicate(anyedge_value*&dup)
{
      anyedge_vec4_value*dup_vec4 = get_vec4_value(dup);
      أكد(dup_vec4);
      dup_vec4->set(old_bits);
}

منطقي anyedge_vec4_value::recv_vec4(ثابت vvp_vector4_t&bit)
{
      منطقي flag = שקר;

      אם (old_bits.size() != bit.size()) {
	    אם (old_bits.size() == 0) {
		    // Special case: If we've not seen any input yet
		    // (old_bits.size()==0) then replace it will a reference
		    // vector that is 'bx. Then compare that with the input
		    // to see if we are processing a change from 'bx.
		  old_bits = vvp_vector4_t(bit.size(), BIT4_X);
		  אם (old_bits.eeq(bit))
			flag = שקר;
		  אחרת
			flag = אמת;

	    } אחרת {
		  flag = אמת;
	    }

      } אחרת {
	    עבור (unsigned idx = 0 ;  idx < bit.size() ;  idx += 1) {
		  אם (old_bits.value(idx) != bit.value(idx)) {
			flag = אמת;
			اكسر;
		  }
	    }
      }

      אם (flag) {
	    old_bits = bit;
      }

      החזר flag;
}

منطقي anyedge_vec4_value::recv_vec4_pv(ثابت vvp_vector4_t&bit, unsigned base,
				      unsigned vwid)
{
      vvp_vector4_t tmp = old_bits;
      אם (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid, BIT4_Z);
      أكد(base + bit.size()<= vwid);
      أكد(tmp.size() == vwid);
      tmp.set_vec(base, bit);

      החזר recv_vec4(tmp);
}

فراغ anyedge_real_value::duplicate(anyedge_value*&dup)
{
      anyedge_real_value*dup_real = get_real_value(dup);
      أكد(dup_real);
      dup_real->set(old_bits);
}

منطقي anyedge_real_value::recv_real(مزدوج bit)
{
      אם (old_bits != bit) {
	    old_bits = bit;
            החזר אמת;
      }
      החזר שקר;
}

فراغ anyedge_string_value::duplicate(anyedge_value*&dup)
{
      anyedge_string_value*dup_string = get_string_value(dup);
      أكد(dup_string);
      dup_string->set(old_bits);
}

منطقي anyedge_string_value::recv_string(ثابت std::string&bit)
{
      אם (old_bits != bit) {
	    old_bits = bit;
            החזר אמת;
      }
      החזר שקר;
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

      החזר tmp;
}

فراغ vvp_fun_anyedge_sa::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                                   vvp_context_t)
{
      anyedge_vec4_value*value = get_vec4_value(last_value_[port.port()]);
      أكد(value);
      אם (value->recv_vec4(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

فراغ vvp_fun_anyedge_sa::recv_vec4_pv(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
				      unsigned base, unsigned vwid, vvp_context_t)
{
      anyedge_vec4_value*value = get_vec4_value(last_value_[port.port()]);
      أكد(value);
      אם (value->recv_vec4_pv(bit, base, vwid)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

فراغ vvp_fun_anyedge_sa::recv_real(vvp_net_ptr_t port, مزدوج bit,
                                   vvp_context_t)
{
      anyedge_real_value*value = get_real_value(last_value_[port.port()]);
      أكد(value);
      אם (value->recv_real(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

فراغ vvp_fun_anyedge_sa::recv_string(vvp_net_ptr_t port, ثابت std::string&bit,
				     vvp_context_t)
{
      anyedge_string_value*value = get_string_value(last_value_[port.port()]);
      أكد(value);
      אם (value->recv_string(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

/*
 * An anyedge receiving an object should do nothing with it, but should
 * trigger waiting threads.
 */
فراغ vvp_fun_anyedge_sa::recv_object(vvp_net_ptr_t port, vvp_object_t,
				     vvp_context_t)
{
      run_waiting_threads_(threads_);
      vvp_net_t*net = port.ptr();
      net->send_vec4(vvp_vector4_t(), 0);
}

vvp_fun_anyedge_aa::vvp_fun_anyedge_aa()
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(هذا, context_scope_);
}

vvp_fun_anyedge_aa::~vvp_fun_anyedge_aa()
{
}

فراغ vvp_fun_anyedge_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, جديد vvp_fun_anyedge_state_s);
      reset_instance(context);
}

فراغ vvp_fun_anyedge_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
      עבור (unsigned idx = 0 ;  idx < 4 ;  idx += 1) {
	    אם (last_value_[idx])
	          last_value_[idx]->duplicate(state->last_value_[idx]);
	    אחרת אם (state->last_value_[idx])
		  state->last_value_[idx]->reset();
      }
}

#ifdef CHECK_WITH_VALGRIND
فراغ vvp_fun_anyedge_aa::free_instance(vvp_context_t context)
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

      החזר tmp;
}

فراغ vvp_fun_anyedge_aa::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      אם (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_vec4_value*value = get_vec4_value(state->last_value_[port.port()]);
            أكد(value);
            אם (value->recv_vec4(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(bit, context);
            }
      } אחרת {
            context = context_scope_->live_contexts;
            כלעוד (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            anyedge_vec4_value*value = get_vec4_value(last_value_[port.port()]);
            أكد(value);
            value->set(bit);
      }
}

فراغ vvp_fun_anyedge_aa::recv_real(vvp_net_ptr_t port, مزدوج bit,
                                   vvp_context_t context)
{
      אם (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_real_value*value = get_real_value(state->last_value_[port.port()]);
            أكد(value);
            אם (value->recv_real(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } אחרת {
            context = context_scope_->live_contexts;
            כלעוד (context) {
                  recv_real(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            anyedge_real_value*value = get_real_value(last_value_[port.port()]);
            أكد(value);
            value->set(bit);
      }
}

فراغ vvp_fun_anyedge_aa::recv_string(vvp_net_ptr_t port, ثابت std::string&bit,
				     vvp_context_t context)
{
      אם (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_string_value*value = get_string_value(state->last_value_[port.port()]);
            أكد(value);
            אם (value->recv_string(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } אחרת {
            context = context_scope_->live_contexts;
            כלעוד (context) {
                  recv_string(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            anyedge_string_value*value = get_string_value(last_value_[port.port()]);
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

      החזר tmp;
}

فراغ vvp_fun_event_or_sa::recv_vec4(vvp_net_ptr_t, ثابت vvp_vector4_t&bit,
                                    vvp_context_t)
{
      run_waiting_threads_(threads_);
      base_net_->send_vec4(bit, 0);
}

vvp_fun_event_or_aa::vvp_fun_event_or_aa(vvp_net_t*base_net)
: vvp_fun_event_or(base_net)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(هذا, context_scope_);
}

vvp_fun_event_or_aa::~vvp_fun_event_or_aa()
{
}

فراغ vvp_fun_event_or_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, جديد waitable_state_s);
}

فراغ vvp_fun_event_or_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
}

#ifdef CHECK_WITH_VALGRIND
فراغ vvp_fun_event_or_aa::free_instance(vvp_context_t context)
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

      החזר tmp;
}

فراغ vvp_fun_event_or_aa::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                                    vvp_context_t context)
{
      אם (context) {
            waitable_state_s*state = static_cast<waitable_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            run_waiting_threads_(state->threads);
            base_net_->send_vec4(bit, context);
      } אחרת {
            context = context_scope_->live_contexts;
            כלעוד (context) {
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

      החזר tmp;
}

فراغ vvp_named_event_sa::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
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
      context_idx_ = vpip_add_item_to_context(هذا, vpip_peek_context_scope());
}

vvp_named_event_aa::~vvp_named_event_aa()
{
}

فراغ vvp_named_event_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, جديد waitable_state_s);
}

فراغ vvp_named_event_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
}

#ifdef CHECK_WITH_VALGRIND
فراغ vvp_named_event_aa::free_instance(vvp_context_t context)
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

      החזר tmp;
}

فراغ vvp_named_event_aa::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
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

ساكن فراغ compile_event_or(حرف*label, unsigned argc, بنية symb_s*argv);

فراغ compile_event(حرف*label, حرف*type, unsigned argc, بنية symb_s*argv)
{
      vvp_net_fun_t*fun = 0;

      אם (type == 0) {
	    compile_event_or(label, argc, argv);
	    החזר;
      }

      אם (strcmp(type,"anyedge") == 0) {

	    حرر(type);

            אם (vpip_peek_current_scope()->is_automatic()) {
                  fun = جديد vvp_fun_anyedge_aa;
            } אחרת {
                  fun = جديد vvp_fun_anyedge_sa;
            }

      } אחרת {

	    vvp_fun_edge::edge_t edge_type = vvp_edge_none;

	    אם (strcmp(type,"posedge") == 0)
		  edge_type = vvp_edge_posedge;
	    אחרת אם (strcmp(type,"negedge") == 0)
		  edge_type = vvp_edge_negedge;
	    אחרת אם (strcmp(type,"edge") == 0)
		  edge_type = vvp_edge_edge;

	    أكد(argc <= 4);
	    حرر(type);

            אם (vpip_peek_current_scope()->is_automatic()) {
                  fun = جديد vvp_fun_edge_aa(edge_type);
            } אחרת {
                  fun = جديد vvp_fun_edge_sa(edge_type);
            }

      }

      vvp_net_t* ptr = جديد vvp_net_t;
      ptr->fun = fun;

      define_functor_symbol(label, ptr);
      حرر(label);

      inputs_connect(ptr, argc, argv);
      حرر(argv);
}

ساكن فراغ compile_event_or(حرف*label, unsigned argc, بنية symb_s*argv)
{
      vvp_net_t*base_net = جديد vvp_net_t;
      אם (vpip_peek_current_scope()->is_automatic()) {
            base_net->fun = جديد vvp_fun_event_or_aa(base_net);
      } אחרת {
            base_net->fun = جديد vvp_fun_event_or_sa(base_net);
      }
      define_functor_symbol(label, base_net);
      حرر(label);

	/* This is a simplified version of a wide functor. We don't
	   care about the data values or what port they arrived on,
	   so we can use a single shared functor. */
      vvp_net_t*curr_net = base_net;
      עבור (unsigned idx = 0 ;  idx < argc ;  idx += 1) {
	    אם (idx > 0 && (idx % 4) == 0) {
		  curr_net = جديد vvp_net_t;
		  curr_net->fun = base_net->fun;
	    }
	    input_connect(curr_net, idx % 4, argv[idx].text);
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

      אם (vpip_peek_current_scope()->is_automatic()) {
            ptr->fun = جديد vvp_named_event_aa(obj);
      } אחרת {
            ptr->fun = جديد vvp_named_event_sa(obj);
      }
      define_functor_symbol(label, ptr);
      compile_vpi_symbol(label, obj);
      אם (! local_flag) vpip_attach_to_current_scope(obj);

      حرر(label);
      delete[] name;
}

#ifdef CHECK_WITH_VALGRIND
فراغ named_event_delete(__vpiHandle*handle)
{
      delete dynamic_cast<__vpiNamedEvent *>(handle);
}
#endif
