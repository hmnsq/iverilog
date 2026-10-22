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

வெறுமை waitable_hooks_s::run_waiting_threads_(vthread_t&threads)
{
	// Run the non-blocking event controls.
      last = &event_ctls;
      க்கு (evctl*cur = event_ctls; cur != 0;) {
	    எனில் (cur->dec_and_run()) {
		  evctl*nxt = cur->next;
		  delete cur;
		  cur = nxt;
		  *last = cur;
	    } இல்லையெனில் {
		  last = &(cur->next);
		  cur = cur->next;
	    }
      }

      vthread_t tmp = threads;
      எனில் (tmp == 0) திரும்பு;
      threads = 0;

      vthread_schedule_list(tmp);
}

evctl::evctl(uint64_t ecount)
{
      ecount_ = ecount;
      next = 0;
}

தருக்க evctl::dec_and_run()
{
      ப்ரதிஜ்ஞா(ecount_ != 0);

      ecount_ -= 1;
      எனில் (ecount_ == 0) run_run();

      திரும்பு ecount_ == 0;
}

evctl_real::evctl_real(__vpiHandle*handle, இரட்டை value,
                       uint64_t ecount)
:evctl(ecount)
{
      handle_ = handle;
      மதிப்பு_2 = value;
}

வெறுமை evctl_real::run_run()
{
      t_vpi_மதிப்பு val;

      val.format = vpiRealVal;
      val.value.real = மதிப்பு_2;
      vpi_put_மதிப்பு(handle_, &val, 0, vpiNoDelay);
}

வெறுமை schedule_evctl(__vpiHandle*handle, இரட்டை value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      ப்ரதிஜ்ஞா(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = புதிய evctl_real(handle, value, ecount);
      ep->last = &((*(ep->last))->next);
}

evctl_vector::evctl_vector(vvp_net_ptr_t ptr, மாறிலி vvp_vector4_t&value,
                           unsigned off, unsigned wid, uint64_t ecount)
:evctl(ecount), ptr_(ptr), மதிப்பு_2(value)
{
      off_ = off;
      wid_ = wid;
}

வெறுமை evctl_vector::run_run()
{
      எனில் (wid_ != 0) {
	    vvp_send_vec4_pv(ptr_, மதிப்பு_2, off_, wid_, 0);
      } இல்லையெனில் {
	    vvp_send_vec4(ptr_, மதிப்பு_2, 0);
      }
}

வெறுமை schedule_evctl(vvp_net_ptr_t ptr, மாறிலி vvp_vector4_t&value,
                    unsigned offset, unsigned wid,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      ப்ரதிஜ்ஞா(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = புதிய evctl_vector(ptr, value, offset, wid, ecount);
      ep->last = &((*(ep->last))->next);
}

evctl_array::evctl_array(vvp_array_t memory, unsigned index,
                         மாறிலி vvp_vector4_t&value, unsigned off,
                         uint64_t ecount)
:evctl(ecount), மதிப்பு_2(value)
{
      mem_ = memory;
      idx_ = index;
      off_ = off;
}

வெறுமை evctl_array::run_run()
{
      mem_->set_word(idx_, off_, மதிப்பு_2);
}

வெறுமை schedule_evctl(vvp_array_t memory, unsigned index,
                    மாறிலி vvp_vector4_t&value, unsigned offset,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      ப்ரதிஜ்ஞா(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = புதிய evctl_array(memory, index, value, offset, ecount);
      ep->last = &((*(ep->last))->next);
}

evctl_array_r::evctl_array_r(vvp_array_t memory, unsigned index,
                             இரட்டை value, uint64_t ecount)
:evctl(ecount)
{
      mem_ = memory;
      idx_ = index;
      மதிப்பு_2 = value;
}

வெறுமை evctl_array_r::run_run()
{
      mem_->set_word(idx_, மதிப்பு_2);
}

வெறுமை schedule_evctl(vvp_array_t memory, unsigned index,
                    இரட்டை value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      ப்ரதிஜ்ஞா(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = புதிய evctl_array_r(memory, index, value, ecount);
      ep->last = &((*(ep->last))->next);
}

உள்வரி vvp_fun_edge::edge_t VVP_EDGE(vvp_bit4_t from, vvp_bit4_t to)
{
      திரும்பு 1 << ((from << 2) | to);
}

மாறிலி vvp_fun_edge::edge_t vvp_edge_posedge
      = VVP_EDGE(BIT4_0,BIT4_1)
      | VVP_EDGE(BIT4_0,BIT4_X)
      | VVP_EDGE(BIT4_0,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_1)
      | VVP_EDGE(BIT4_Z,BIT4_1)
      ;

மாறிலி vvp_fun_edge::edge_t vvp_edge_negedge
      = VVP_EDGE(BIT4_1,BIT4_0)
      | VVP_EDGE(BIT4_1,BIT4_X)
      | VVP_EDGE(BIT4_1,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_0)
      | VVP_EDGE(BIT4_Z,BIT4_0)
      ;

மாறிலி vvp_fun_edge::edge_t vvp_edge_edge
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

மாறிலி vvp_fun_edge::edge_t vvp_edge_none    = 0;

கட்டமைப்பு vvp_fun_edge_state_s : பொது waitable_state_s {
      vvp_fun_edge_state_s()
      {
            க்கு (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
                  bits[idx] = BIT4_X;
      }

      vvp_bit4_t bits[4];
};

vvp_fun_edge::vvp_fun_edge(edge_t e)
: edge_(e)
{
      க்கு (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
            bits_[idx] = BIT4_X;
}

vvp_fun_edge::~vvp_fun_edge()
{
}

தருக்க vvp_fun_edge::recv_vec4_(மாறிலி vvp_vector4_t&bit,
                              vvp_bit4_t&old_bit, vthread_t&threads)
{
	/* See what kind of edge this represents. */
      edge_t mask = VVP_EDGE(old_bit, bit.value(0));

	/* Save the current input for the next time around. */
      old_bit = bit.value(0);

      எனில் ((edge_ == vvp_edge_none) || (edge_ & mask)) {
	    run_waiting_threads_(threads);
            திரும்பு உண்மை;
      }
      திரும்பு பொய்;
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

      திரும்பு tmp;
}

வெறுமை vvp_fun_edge_sa::recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                                vvp_context_t)
{
      எனில் (recv_vec4_(bit, bits_[port.port()], threads_)) {
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

வெறுமை vvp_fun_edge_sa::recv_vec4_pv(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t)
{
      ப்ரதிஜ்ஞா(base == 0);
      எனில் (recv_vec4_(bit, bits_[port.port()], threads_)) {
	    vvp_net_t*net = port.ptr();
	    net->send_vec4_pv(bit, base, vwid, 0);
      }
}

vvp_fun_edge_aa::vvp_fun_edge_aa(edge_t e)
: vvp_fun_edge(e)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(இது, context_scope_);
}

vvp_fun_edge_aa::~vvp_fun_edge_aa()
{
}

வெறுமை vvp_fun_edge_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, புதிய vvp_fun_edge_state_s);
      reset_instance(context);
}

வெறுமை vvp_fun_edge_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_edge_state_s*state = static_cast<vvp_fun_edge_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
      க்கு (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
            state->bits[idx] = bits_[idx];
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை vvp_fun_edge_aa::free_instance(vvp_context_t context)
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

      திரும்பு tmp;
}

வெறுமை vvp_fun_edge_aa::recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                                vvp_context_t context)
{
      எனில் (context) {
            vvp_fun_edge_state_s*state = static_cast<vvp_fun_edge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            எனில் (recv_vec4_(bit, state->bits[port.port()], state->threads)) {
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(bit, context);
            }
      } இல்லையெனில் {
            context = context_scope_->live_contexts;
            போது (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            bits_[port.port()] = bit.value(0);
      }
}

வகுப்பு anyedge_மதிப்பு {

    பொது:
      anyedge_மதிப்பு() {};
      மெய்நிகர் ~anyedge_மதிப்பு() {};

      மெய்நிகர் வெறுமை reset() = 0;

      மெய்நிகர் வெறுமை duplicate(anyedge_மதிப்பு*&dup) = 0;
};

வகுப்பு anyedge_vec4_மதிப்பு : பொது anyedge_மதிப்பு {

    பொது:
      anyedge_vec4_மதிப்பு() {};
      மெய்நிகர் ~anyedge_vec4_மதிப்பு() override {};

      வெறுமை reset() override { old_bits.set_to_x(); }

      வெறுமை set(மாறிலி vvp_vector4_t&bit) { old_bits = bit; };

      வெறுமை duplicate(anyedge_மதிப்பு*&dup) override;

      தருக்க recv_vec4(மாறிலி vvp_vector4_t&bit);

      தருக்க recv_vec4_pv(மாறிலி vvp_vector4_t&bit, unsigned base,
			unsigned vwid);

    தனிப்பட்ட:
      vvp_vector4_t old_bits;
};

நிலையான anyedge_vec4_மதிப்பு*get_vec4_மதிப்பு(anyedge_மதிப்பு*&value)
{
      anyedge_vec4_மதிப்பு*vec4_மதிப்பு = dynamic_cast<anyedge_vec4_மதிப்பு*>(value);
      எனில் (!value) {
	    vec4_மதிப்பு = புதிய anyedge_vec4_மதிப்பு();
	    delete value;
	    value = vec4_மதிப்பு;
      }
      திரும்பு vec4_மதிப்பு;
}

வகுப்பு anyedge_real_மதிப்பு : பொது anyedge_மதிப்பு {

    பொது:
      anyedge_real_மதிப்பு() : old_bits(0.0) {};
      மெய்நிகர் ~anyedge_real_மதிப்பு() override {};

      வெறுமை reset() override { old_bits = 0.0; }

      வெறுமை set(இரட்டை bit) { old_bits = bit; };

      வெறுமை duplicate(anyedge_மதிப்பு*&dup) override;

      தருக்க recv_real(இரட்டை bit);

    தனிப்பட்ட:
      இரட்டை old_bits;
};

நிலையான anyedge_real_மதிப்பு*get_real_மதிப்பு(anyedge_மதிப்பு*&value)
{
      anyedge_real_மதிப்பு*real_மதிப்பு = dynamic_cast<anyedge_real_மதிப்பு*>(value);
      எனில் (!value) {
	    real_மதிப்பு = புதிய anyedge_real_மதிப்பு();
	    delete value;
	    value = real_மதிப்பு;
      }
      திரும்பு real_மதிப்பு;
}

வகுப்பு anyedge_string_மதிப்பு : பொது anyedge_மதிப்பு {

    பொது:
      anyedge_string_மதிப்பு() {};
      மெய்நிகர் ~anyedge_string_மதிப்பு() override {};

      வெறுமை reset() override { old_bits.clear(); }

      வெறுமை set(மாறிலி std::string&bit) { old_bits = bit; };

      வெறுமை duplicate(anyedge_மதிப்பு*&dup) override;

      தருக்க recv_string(மாறிலி std::string&bit);

    தனிப்பட்ட:
      std::string old_bits;
};

நிலையான anyedge_string_மதிப்பு*get_string_மதிப்பு(anyedge_மதிப்பு*&value)
{
      anyedge_string_மதிப்பு*string_மதிப்பு = dynamic_cast<anyedge_string_மதிப்பு*>(value);
      எனில் (!value) {
	    string_மதிப்பு = புதிய anyedge_string_மதிப்பு();
	    delete value;
	    value = string_மதிப்பு;
      }
      திரும்பு string_மதிப்பு;
}

கட்டமைப்பு vvp_fun_anyedge_state_s : பொது waitable_state_s {
      vvp_fun_anyedge_state_s()
      {
            க்கு (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
                  last_மதிப்பு[idx] = 0;
      }

      ~vvp_fun_anyedge_state_s()
      {
            க்கு (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
                  delete last_மதிப்பு[idx];
      }

      anyedge_மதிப்பு *last_மதிப்பு[4];
      vvp_fun_anyedge_state_s(மாறிலி vvp_fun_anyedge_state_s&) = delete;
      vvp_fun_anyedge_state_s& operator=(மாறிலி vvp_fun_anyedge_state_s&) = delete;
};

vvp_fun_anyedge::vvp_fun_anyedge()
{
      க்கு (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
	    last_மதிப்பு[idx] = 0;
}

vvp_fun_anyedge::~vvp_fun_anyedge()
{
      க்கு (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
	    delete last_மதிப்பு[idx];
}

வெறுமை anyedge_vec4_மதிப்பு::duplicate(anyedge_மதிப்பு*&dup)
{
      anyedge_vec4_மதிப்பு*dup_vec4 = get_vec4_மதிப்பு(dup);
      ப்ரதிஜ்ஞா(dup_vec4);
      dup_vec4->set(old_bits);
}

தருக்க anyedge_vec4_மதிப்பு::recv_vec4(மாறிலி vvp_vector4_t&bit)
{
      தருக்க flag = பொய்;

      எனில் (old_bits.size() != bit.size()) {
	    எனில் (old_bits.size() == 0) {
		    // Special case: If we've not seen any input yet
		    // (old_bits.size()==0) then replace it will a reference
		    // vector that is 'bx. Then compare that with the input
		    // to see if we are processing a change from 'bx.
		  old_bits = vvp_vector4_t(bit.size(), BIT4_X);
		  எனில் (old_bits.eeq(bit))
			flag = பொய்;
		  இல்லையெனில்
			flag = உண்மை;

	    } இல்லையெனில் {
		  flag = உண்மை;
	    }

      } இல்லையெனில் {
	    க்கு (unsigned idx = 0 ;  idx < bit.size() ;  idx += 1) {
		  எனில் (old_bits.value(idx) != bit.value(idx)) {
			flag = உண்மை;
			நிறுத்து;
		  }
	    }
      }

      எனில் (flag) {
	    old_bits = bit;
      }

      திரும்பு flag;
}

தருக்க anyedge_vec4_மதிப்பு::recv_vec4_pv(மாறிலி vvp_vector4_t&bit, unsigned base,
				      unsigned vwid)
{
      vvp_vector4_t tmp = old_bits;
      எனில் (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid, BIT4_Z);
      ப்ரதிஜ்ஞா(base + bit.size()<= vwid);
      ப்ரதிஜ்ஞா(tmp.size() == vwid);
      tmp.set_vec(base, bit);

      திரும்பு recv_vec4(tmp);
}

வெறுமை anyedge_real_மதிப்பு::duplicate(anyedge_மதிப்பு*&dup)
{
      anyedge_real_மதிப்பு*dup_real = get_real_மதிப்பு(dup);
      ப்ரதிஜ்ஞா(dup_real);
      dup_real->set(old_bits);
}

தருக்க anyedge_real_மதிப்பு::recv_real(இரட்டை bit)
{
      எனில் (old_bits != bit) {
	    old_bits = bit;
            திரும்பு உண்மை;
      }
      திரும்பு பொய்;
}

வெறுமை anyedge_string_மதிப்பு::duplicate(anyedge_மதிப்பு*&dup)
{
      anyedge_string_மதிப்பு*dup_string = get_string_மதிப்பு(dup);
      ப்ரதிஜ்ஞா(dup_string);
      dup_string->set(old_bits);
}

தருக்க anyedge_string_மதிப்பு::recv_string(மாறிலி std::string&bit)
{
      எனில் (old_bits != bit) {
	    old_bits = bit;
            திரும்பு உண்மை;
      }
      திரும்பு பொய்;
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

      திரும்பு tmp;
}

வெறுமை vvp_fun_anyedge_sa::recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                                   vvp_context_t)
{
      anyedge_vec4_மதிப்பு*value = get_vec4_மதிப்பு(last_மதிப்பு[port.port()]);
      ப்ரதிஜ்ஞா(value);
      எனில் (value->recv_vec4(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

வெறுமை vvp_fun_anyedge_sa::recv_vec4_pv(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
				      unsigned base, unsigned vwid, vvp_context_t)
{
      anyedge_vec4_மதிப்பு*value = get_vec4_மதிப்பு(last_மதிப்பு[port.port()]);
      ப்ரதிஜ்ஞா(value);
      எனில் (value->recv_vec4_pv(bit, base, vwid)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

வெறுமை vvp_fun_anyedge_sa::recv_real(vvp_net_ptr_t port, இரட்டை bit,
                                   vvp_context_t)
{
      anyedge_real_மதிப்பு*value = get_real_மதிப்பு(last_மதிப்பு[port.port()]);
      ப்ரதிஜ்ஞா(value);
      எனில் (value->recv_real(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

வெறுமை vvp_fun_anyedge_sa::recv_string(vvp_net_ptr_t port, மாறிலி std::string&bit,
				     vvp_context_t)
{
      anyedge_string_மதிப்பு*value = get_string_மதிப்பு(last_மதிப்பு[port.port()]);
      ப்ரதிஜ்ஞா(value);
      எனில் (value->recv_string(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

/*
 * An anyedge receiving an object should do nothing with it, but should
 * trigger waiting threads.
 */
வெறுமை vvp_fun_anyedge_sa::recv_object(vvp_net_ptr_t port, vvp_object_t,
				     vvp_context_t)
{
      run_waiting_threads_(threads_);
      vvp_net_t*net = port.ptr();
      net->send_vec4(vvp_vector4_t(), 0);
}

vvp_fun_anyedge_aa::vvp_fun_anyedge_aa()
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(இது, context_scope_);
}

vvp_fun_anyedge_aa::~vvp_fun_anyedge_aa()
{
}

வெறுமை vvp_fun_anyedge_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, புதிய vvp_fun_anyedge_state_s);
      reset_instance(context);
}

வெறுமை vvp_fun_anyedge_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
      க்கு (unsigned idx = 0 ;  idx < 4 ;  idx += 1) {
	    எனில் (last_மதிப்பு[idx])
	          last_மதிப்பு[idx]->duplicate(state->last_மதிப்பு[idx]);
	    இல்லையெனில் எனில் (state->last_மதிப்பு[idx])
		  state->last_மதிப்பு[idx]->reset();
      }
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை vvp_fun_anyedge_aa::free_instance(vvp_context_t context)
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

      திரும்பு tmp;
}

வெறுமை vvp_fun_anyedge_aa::recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      எனில் (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_vec4_மதிப்பு*value = get_vec4_மதிப்பு(state->last_மதிப்பு[port.port()]);
            ப்ரதிஜ்ஞா(value);
            எனில் (value->recv_vec4(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(bit, context);
            }
      } இல்லையெனில் {
            context = context_scope_->live_contexts;
            போது (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            anyedge_vec4_மதிப்பு*value = get_vec4_மதிப்பு(last_மதிப்பு[port.port()]);
            ப்ரதிஜ்ஞா(value);
            value->set(bit);
      }
}

வெறுமை vvp_fun_anyedge_aa::recv_real(vvp_net_ptr_t port, இரட்டை bit,
                                   vvp_context_t context)
{
      எனில் (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_real_மதிப்பு*value = get_real_மதிப்பு(state->last_மதிப்பு[port.port()]);
            ப்ரதிஜ்ஞா(value);
            எனில் (value->recv_real(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } இல்லையெனில் {
            context = context_scope_->live_contexts;
            போது (context) {
                  recv_real(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            anyedge_real_மதிப்பு*value = get_real_மதிப்பு(last_மதிப்பு[port.port()]);
            ப்ரதிஜ்ஞா(value);
            value->set(bit);
      }
}

வெறுமை vvp_fun_anyedge_aa::recv_string(vvp_net_ptr_t port, மாறிலி std::string&bit,
				     vvp_context_t context)
{
      எனில் (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_string_மதிப்பு*value = get_string_மதிப்பு(state->last_மதிப்பு[port.port()]);
            ப்ரதிஜ்ஞா(value);
            எனில் (value->recv_string(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } இல்லையெனில் {
            context = context_scope_->live_contexts;
            போது (context) {
                  recv_string(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            anyedge_string_மதிப்பு*value = get_string_மதிப்பு(last_மதிப்பு[port.port()]);
            ப்ரதிஜ்ஞா(value);
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

      திரும்பு tmp;
}

வெறுமை vvp_fun_event_or_sa::recv_vec4(vvp_net_ptr_t, மாறிலி vvp_vector4_t&bit,
                                    vvp_context_t)
{
      run_waiting_threads_(threads_);
      base_net_->send_vec4(bit, 0);
}

vvp_fun_event_or_aa::vvp_fun_event_or_aa(vvp_net_t*base_net)
: vvp_fun_event_or(base_net)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(இது, context_scope_);
}

vvp_fun_event_or_aa::~vvp_fun_event_or_aa()
{
}

வெறுமை vvp_fun_event_or_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, புதிய waitable_state_s);
}

வெறுமை vvp_fun_event_or_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை vvp_fun_event_or_aa::free_instance(vvp_context_t context)
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

      திரும்பு tmp;
}

வெறுமை vvp_fun_event_or_aa::recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                                    vvp_context_t context)
{
      எனில் (context) {
            waitable_state_s*state = static_cast<waitable_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            run_waiting_threads_(state->threads);
            base_net_->send_vec4(bit, context);
      } இல்லையெனில் {
            context = context_scope_->live_contexts;
            போது (context) {
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

      திரும்பு tmp;
}

வெறுமை vvp_named_event_sa::recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                                   vvp_context_t)
{
      run_waiting_threads_(threads_);
      vvp_net_t*net = port.ptr();
      net->send_vec4(bit, 0);

      __vpiNamedEvent*obj = dynamic_cast<__vpiNamedEvent*>(handle_);
      ப்ரதிஜ்ஞா(obj);
      obj->run_vpi_callbacks();
}

vvp_named_event_aa::vvp_named_event_aa(__vpiHandle*h)
: vvp_named_event(h)
{
      context_idx_ = vpip_add_item_to_context(இது, vpip_peek_context_scope());
}

vvp_named_event_aa::~vvp_named_event_aa()
{
}

வெறுமை vvp_named_event_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, புதிய waitable_state_s);
}

வெறுமை vvp_named_event_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை vvp_named_event_aa::free_instance(vvp_context_t context)
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

      திரும்பு tmp;
}

வெறுமை vvp_named_event_aa::recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      ப்ரதிஜ்ஞா(context);

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

நிலையான வெறுமை compile_event_or(எழுத்து*label, unsigned argc, கட்டமைப்பு symb_s*argv);

வெறுமை compile_event(எழுத்து*label, எழுத்து*type, unsigned argc, கட்டமைப்பு symb_s*argv)
{
      vvp_net_fun_t*fun = 0;

      எனில் (type == 0) {
	    compile_event_or(label, argc, argv);
	    திரும்பு;
      }

      எனில் (strcmp(type,"anyedge") == 0) {

	    விடுவி(type);

            எனில் (vpip_peek_current_scope()->is_automatic()) {
                  fun = புதிய vvp_fun_anyedge_aa;
            } இல்லையெனில் {
                  fun = புதிய vvp_fun_anyedge_sa;
            }

      } இல்லையெனில் {

	    vvp_fun_edge::edge_t edge_type = vvp_edge_none;

	    எனில் (strcmp(type,"posedge") == 0)
		  edge_type = vvp_edge_posedge;
	    இல்லையெனில் எனில் (strcmp(type,"negedge") == 0)
		  edge_type = vvp_edge_negedge;
	    இல்லையெனில் எனில் (strcmp(type,"edge") == 0)
		  edge_type = vvp_edge_edge;

	    ப்ரதிஜ்ஞா(argc <= 4);
	    விடுவி(type);

            எனில் (vpip_peek_current_scope()->is_automatic()) {
                  fun = புதிய vvp_fun_edge_aa(edge_type);
            } இல்லையெனில் {
                  fun = புதிய vvp_fun_edge_sa(edge_type);
            }

      }

      vvp_net_t* ptr = புதிய vvp_net_t;
      ptr->fun = fun;

      define_functor_குறியீடு(label, ptr);
      விடுவி(label);

      inputs_connect(ptr, argc, argv);
      விடுவி(argv);
}

நிலையான வெறுமை compile_event_or(எழுத்து*label, unsigned argc, கட்டமைப்பு symb_s*argv)
{
      vvp_net_t*base_net = புதிய vvp_net_t;
      எனில் (vpip_peek_current_scope()->is_automatic()) {
            base_net->fun = புதிய vvp_fun_event_or_aa(base_net);
      } இல்லையெனில் {
            base_net->fun = புதிய vvp_fun_event_or_sa(base_net);
      }
      define_functor_குறியீடு(label, base_net);
      விடுவி(label);

	/* This is a simplified version of a wide functor. We don't
	   care about the data values or what port they arrived on,
	   so we can use a single shared functor. */
      vvp_net_t*curr_net = base_net;
      க்கு (unsigned idx = 0 ;  idx < argc ;  idx += 1) {
	    எனில் (idx > 0 && (idx % 4) == 0) {
		  curr_net = புதிய vvp_net_t;
		  curr_net->fun = base_net->fun;
	    }
	    உள்ளீடு_connect(curr_net, idx % 4, argv[idx].text);
      }
      விடுவி(argv);
}

/*
 * This handles the compile of named events. This functor has no
 * inputs, it is only accessed by behavioral trigger statements, which
 * in vvp are %set instructions.
 */
வெறுமை compile_named_event(எழுத்து*label, எழுத்து*name, தருக்க local_flag)
{
      vvp_net_t*ptr = புதிய vvp_net_t;

      vpiHandle obj = vpip_make_named_event(name, ptr);

      எனில் (vpip_peek_current_scope()->is_automatic()) {
            ptr->fun = புதிய vvp_named_event_aa(obj);
      } இல்லையெனில் {
            ptr->fun = புதிய vvp_named_event_sa(obj);
      }
      define_functor_குறியீடு(label, ptr);
      compile_vpi_குறியீடு(label, obj);
      எனில் (! local_flag) vpip_attach_to_current_scope(obj);

      விடுவி(label);
      delete[] name;
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை named_event_delete(__vpiHandle*handle)
{
      delete dynamic_cast<__vpiNamedEvent *>(handle);
}
#endif
