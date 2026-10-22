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
      لكل (evctl*cur = event_ctls; cur != 0;) {
	    إذا (cur->dec_and_run()) {
		  evctl*nxt = cur->next;
		  delete cur;
		  cur = nxt;
		  *last = cur;
	    } وإلا {
		  last = &(cur->next);
		  cur = cur->next;
	    }
      }

      vthread_t tmp = threads;
      إذا (tmp == 0) أرجع;
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
      إذا (ecount_ == 0) run_run();

      أرجع ecount_ == 0;
}

evctl_real::evctl_real(__vpiHandle*handle, مزدوج value,
                       uint64_t ecount)
:evctl(ecount)
{
      handle_ = handle;
      درج_2 = value;
}

فراغ evctl_real::run_run()
{
      t_vpi_درج val;

      val.format = vpiRealVal;
      val.value.real = درج_2;
      vpi_put_درج(handle_, &val, 0, vpiNoDelay);
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
:evctl(ecount), ptr_(ptr), درج_2(value)
{
      off_ = off;
      wid_ = wid;
}

فراغ evctl_vector::run_run()
{
      إذا (wid_ != 0) {
	    vvp_send_vec4_pv(ptr_, درج_2, off_, wid_, 0);
      } وإلا {
	    vvp_send_vec4(ptr_, درج_2, 0);
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
:evctl(ecount), درج_2(value)
{
      mem_ = memory;
      idx_ = index;
      off_ = off;
}

فراغ evctl_array::run_run()
{
      mem_->set_word(idx_, off_, درج_2);
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
      درج_2 = value;
}

فراغ evctl_array_r::run_run()
{
      mem_->set_word(idx_, درج_2);
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
      أرجع 1 << ((from << 2) | to);
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
            لكل (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
                  bits[idx] = BIT4_X;
      }

      vvp_bit4_t bits[4];
};

vvp_fun_edge::vvp_fun_edge(edge_t e)
: edge_(e)
{
      لكل (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
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

      إذا ((edge_ == vvp_edge_none) || (edge_ & mask)) {
	    run_waiting_threads_(threads);
            أرجع true;
      }
      أرجع خطأ;
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

      أرجع tmp;
}

فراغ vvp_fun_edge_sa::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                                vvp_context_t)
{
      إذا (recv_vec4_(bit, bits_[port.port()], threads_)) {
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

فراغ vvp_fun_edge_sa::recv_vec4_pv(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t)
{
      أكد(base == 0);
      إذا (recv_vec4_(bit, bits_[port.port()], threads_)) {
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
      لكل (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
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

      أرجع tmp;
}

فراغ vvp_fun_edge_aa::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                                vvp_context_t context)
{
      إذا (context) {
            vvp_fun_edge_state_s*state = static_cast<vvp_fun_edge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            إذا (recv_vec4_(bit, state->bits[port.port()], state->threads)) {
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(bit, context);
            }
      } وإلا {
            context = context_scope_->live_contexts;
            طالما (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            bits_[port.port()] = bit.value(0);
      }
}

صنف anyedge_درج {

    عام:
      anyedge_درج() {};
      virtual ~anyedge_درج() {};

      virtual فراغ reset() = 0;

      virtual فراغ duplicate(anyedge_درج*&dup) = 0;
};

صنف anyedge_vec4_درج : عام anyedge_درج {

    عام:
      anyedge_vec4_درج() {};
      virtual ~anyedge_vec4_درج() override {};

      فراغ reset() override { old_bits.set_to_x(); }

      فراغ set(ثابت vvp_vector4_t&bit) { old_bits = bit; };

      فراغ duplicate(anyedge_درج*&dup) override;

      منطقي recv_vec4(ثابت vvp_vector4_t&bit);

      منطقي recv_vec4_pv(ثابت vvp_vector4_t&bit, unsigned base,
			unsigned vwid);

    خاص:
      vvp_vector4_t old_bits;
};

ساكن anyedge_vec4_درج*get_vec4_درج(anyedge_درج*&value)
{
      anyedge_vec4_درج*vec4_درج = dynamic_cast<anyedge_vec4_درج*>(value);
      إذا (!value) {
	    vec4_درج = جديد anyedge_vec4_درج();
	    delete value;
	    value = vec4_درج;
      }
      أرجع vec4_درج;
}

صنف anyedge_real_درج : عام anyedge_درج {

    عام:
      anyedge_real_درج() : old_bits(0.0) {};
      virtual ~anyedge_real_درج() override {};

      فراغ reset() override { old_bits = 0.0; }

      فراغ set(مزدوج bit) { old_bits = bit; };

      فراغ duplicate(anyedge_درج*&dup) override;

      منطقي recv_real(مزدوج bit);

    خاص:
      مزدوج old_bits;
};

ساكن anyedge_real_درج*get_real_درج(anyedge_درج*&value)
{
      anyedge_real_درج*real_درج = dynamic_cast<anyedge_real_درج*>(value);
      إذا (!value) {
	    real_درج = جديد anyedge_real_درج();
	    delete value;
	    value = real_درج;
      }
      أرجع real_درج;
}

صنف anyedge_string_درج : عام anyedge_درج {

    عام:
      anyedge_string_درج() {};
      virtual ~anyedge_string_درج() override {};

      فراغ reset() override { old_bits.clear(); }

      فراغ set(ثابت std::string&bit) { old_bits = bit; };

      فراغ duplicate(anyedge_درج*&dup) override;

      منطقي recv_string(ثابت std::string&bit);

    خاص:
      std::string old_bits;
};

ساكن anyedge_string_درج*get_string_درج(anyedge_درج*&value)
{
      anyedge_string_درج*string_درج = dynamic_cast<anyedge_string_درج*>(value);
      إذا (!value) {
	    string_درج = جديد anyedge_string_درج();
	    delete value;
	    value = string_درج;
      }
      أرجع string_درج;
}

بنية vvp_fun_anyedge_state_s : عام waitable_state_s {
      vvp_fun_anyedge_state_s()
      {
            لكل (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
                  last_درج[idx] = 0;
      }

      ~vvp_fun_anyedge_state_s()
      {
            لكل (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
                  delete last_درج[idx];
      }

      anyedge_درج *last_درج[4];
      vvp_fun_anyedge_state_s(ثابت vvp_fun_anyedge_state_s&) = delete;
      vvp_fun_anyedge_state_s& operator=(ثابت vvp_fun_anyedge_state_s&) = delete;
};

vvp_fun_anyedge::vvp_fun_anyedge()
{
      لكل (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
	    last_درج[idx] = 0;
}

vvp_fun_anyedge::~vvp_fun_anyedge()
{
      لكل (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
	    delete last_درج[idx];
}

فراغ anyedge_vec4_درج::duplicate(anyedge_درج*&dup)
{
      anyedge_vec4_درج*dup_vec4 = get_vec4_درج(dup);
      أكد(dup_vec4);
      dup_vec4->set(old_bits);
}

منطقي anyedge_vec4_درج::recv_vec4(ثابت vvp_vector4_t&bit)
{
      منطقي flag = خطأ;

      إذا (old_bits.size() != bit.size()) {
	    إذا (old_bits.size() == 0) {
		    // Special case: If we've not seen any input yet
		    // (old_bits.size()==0) then replace it will a reference
		    // vector that is 'bx. Then compare that with the input
		    // to see if we are processing a change from 'bx.
		  old_bits = vvp_vector4_t(bit.size(), BIT4_X);
		  إذا (old_bits.eeq(bit))
			flag = خطأ;
		  وإلا
			flag = true;

	    } وإلا {
		  flag = true;
	    }

      } وإلا {
	    لكل (unsigned idx = 0 ;  idx < bit.size() ;  idx += 1) {
		  إذا (old_bits.value(idx) != bit.value(idx)) {
			flag = true;
			اكسر;
		  }
	    }
      }

      إذا (flag) {
	    old_bits = bit;
      }

      أرجع flag;
}

منطقي anyedge_vec4_درج::recv_vec4_pv(ثابت vvp_vector4_t&bit, unsigned base,
				      unsigned vwid)
{
      vvp_vector4_t tmp = old_bits;
      إذا (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid, BIT4_Z);
      أكد(base + bit.size()<= vwid);
      أكد(tmp.size() == vwid);
      tmp.set_vec(base, bit);

      أرجع recv_vec4(tmp);
}

فراغ anyedge_real_درج::duplicate(anyedge_درج*&dup)
{
      anyedge_real_درج*dup_real = get_real_درج(dup);
      أكد(dup_real);
      dup_real->set(old_bits);
}

منطقي anyedge_real_درج::recv_real(مزدوج bit)
{
      إذا (old_bits != bit) {
	    old_bits = bit;
            أرجع true;
      }
      أرجع خطأ;
}

فراغ anyedge_string_درج::duplicate(anyedge_درج*&dup)
{
      anyedge_string_درج*dup_string = get_string_درج(dup);
      أكد(dup_string);
      dup_string->set(old_bits);
}

منطقي anyedge_string_درج::recv_string(ثابت std::string&bit)
{
      إذا (old_bits != bit) {
	    old_bits = bit;
            أرجع true;
      }
      أرجع خطأ;
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

      أرجع tmp;
}

فراغ vvp_fun_anyedge_sa::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                                   vvp_context_t)
{
      anyedge_vec4_درج*value = get_vec4_درج(last_درج[port.port()]);
      أكد(value);
      إذا (value->recv_vec4(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

فراغ vvp_fun_anyedge_sa::recv_vec4_pv(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
				      unsigned base, unsigned vwid, vvp_context_t)
{
      anyedge_vec4_درج*value = get_vec4_درج(last_درج[port.port()]);
      أكد(value);
      إذا (value->recv_vec4_pv(bit, base, vwid)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

فراغ vvp_fun_anyedge_sa::recv_real(vvp_net_ptr_t port, مزدوج bit,
                                   vvp_context_t)
{
      anyedge_real_درج*value = get_real_درج(last_درج[port.port()]);
      أكد(value);
      إذا (value->recv_real(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

فراغ vvp_fun_anyedge_sa::recv_string(vvp_net_ptr_t port, ثابت std::string&bit,
				     vvp_context_t)
{
      anyedge_string_درج*value = get_string_درج(last_درج[port.port()]);
      أكد(value);
      إذا (value->recv_string(bit)) {
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
      لكل (unsigned idx = 0 ;  idx < 4 ;  idx += 1) {
	    إذا (last_درج[idx])
	          last_درج[idx]->duplicate(state->last_درج[idx]);
	    وإلا إذا (state->last_درج[idx])
		  state->last_درج[idx]->reset();
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

      أرجع tmp;
}

فراغ vvp_fun_anyedge_aa::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      إذا (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_vec4_درج*value = get_vec4_درج(state->last_درج[port.port()]);
            أكد(value);
            إذا (value->recv_vec4(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(bit, context);
            }
      } وإلا {
            context = context_scope_->live_contexts;
            طالما (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            anyedge_vec4_درج*value = get_vec4_درج(last_درج[port.port()]);
            أكد(value);
            value->set(bit);
      }
}

فراغ vvp_fun_anyedge_aa::recv_real(vvp_net_ptr_t port, مزدوج bit,
                                   vvp_context_t context)
{
      إذا (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_real_درج*value = get_real_درج(state->last_درج[port.port()]);
            أكد(value);
            إذا (value->recv_real(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } وإلا {
            context = context_scope_->live_contexts;
            طالما (context) {
                  recv_real(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            anyedge_real_درج*value = get_real_درج(last_درج[port.port()]);
            أكد(value);
            value->set(bit);
      }
}

فراغ vvp_fun_anyedge_aa::recv_string(vvp_net_ptr_t port, ثابت std::string&bit,
				     vvp_context_t context)
{
      إذا (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_string_درج*value = get_string_درج(state->last_درج[port.port()]);
            أكد(value);
            إذا (value->recv_string(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } وإلا {
            context = context_scope_->live_contexts;
            طالما (context) {
                  recv_string(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            anyedge_string_درج*value = get_string_درج(last_درج[port.port()]);
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

      أرجع tmp;
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

      أرجع tmp;
}

فراغ vvp_fun_event_or_aa::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                                    vvp_context_t context)
{
      إذا (context) {
            waitable_state_s*state = static_cast<waitable_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            run_waiting_threads_(state->threads);
            base_net_->send_vec4(bit, context);
      } وإلا {
            context = context_scope_->live_contexts;
            طالما (context) {
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

      أرجع tmp;
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

      أرجع tmp;
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

	    vvp_fun_edge::edge_t edge_type = vvp_edge_none;

	    إذا (strcmp(type,"posedge") == 0)
		  edge_type = vvp_edge_posedge;
	    وإلا إذا (strcmp(type,"negedge") == 0)
		  edge_type = vvp_edge_negedge;
	    وإلا إذا (strcmp(type,"edge") == 0)
		  edge_type = vvp_edge_edge;

	    أكد(argc <= 4);
	    حرر(type);

            إذا (vpip_peek_current_scope()->is_automatic()) {
                  fun = جديد vvp_fun_edge_aa(edge_type);
            } وإلا {
                  fun = جديد vvp_fun_edge_sa(edge_type);
            }

      }

      vvp_net_t* ptr = جديد vvp_net_t;
      ptr->fun = fun;

      define_functor_علام(label, ptr);
      حرر(label);

      inputs_connect(ptr, argc, argv);
      حرر(argv);
}

ساكن فراغ compile_event_or(حرف*label, unsigned argc, بنية symb_s*argv)
{
      vvp_net_t*base_net = جديد vvp_net_t;
      إذا (vpip_peek_current_scope()->is_automatic()) {
            base_net->fun = جديد vvp_fun_event_or_aa(base_net);
      } وإلا {
            base_net->fun = جديد vvp_fun_event_or_sa(base_net);
      }
      define_functor_علام(label, base_net);
      حرر(label);

	/* This is a simplified version of a wide functor. We don't
	   care about the data values or what port they arrived on,
	   so we can use a single shared functor. */
      vvp_net_t*curr_net = base_net;
      لكل (unsigned idx = 0 ;  idx < argc ;  idx += 1) {
	    إذا (idx > 0 && (idx % 4) == 0) {
		  curr_net = جديد vvp_net_t;
		  curr_net->fun = base_net->fun;
	    }
	    شغرو_connect(curr_net, idx % 4, argv[idx].text);
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
      define_functor_علام(label, ptr);
      compile_vpi_علام(label, obj);
      إذا (! local_flag) vpip_attach_to_current_scope(obj);

      حرر(label);
      delete[] name;
}

#ifdef CHECK_WITH_VALGRIND
فراغ named_event_delete(__vpiHandle*handle)
{
      delete dynamic_cast<__vpiNamedEvent *>(handle);
}
#endif
