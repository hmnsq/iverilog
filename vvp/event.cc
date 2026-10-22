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

शून्य waitable_hooks_s::run_waiting_threads_(vthread_t&threads)
{
	// Run the non-blocking event controls.
      last = &event_ctls;
      प्रति (evctl*cur = event_ctls; cur != 0;) {
	    यदि (cur->dec_and_run()) {
		  evctl*nxt = cur->next;
		  delete cur;
		  cur = nxt;
		  *last = cur;
	    } अन्यथा {
		  last = &(cur->next);
		  cur = cur->next;
	    }
      }

      vthread_t tmp = threads;
      यदि (tmp == 0) प्रत्यागम;
      threads = 0;

      vthread_schedule_list(tmp);
}

evctl::evctl(uint64_t ecount)
{
      ecount_ = ecount;
      next = 0;
}

तार्किक evctl::dec_and_run()
{
      प्रतिज्ञा(ecount_ != 0);

      ecount_ -= 1;
      यदि (ecount_ == 0) run_run();

      प्रत्यागम ecount_ == 0;
}

evctl_real::evctl_real(__vpiHandle*handle, द्विगुण value,
                       uint64_t ecount)
:evctl(ecount)
{
      handle_ = handle;
      मूल्य_2 = value;
}

शून्य evctl_real::run_run()
{
      t_vpi_मूल्य val;

      val.format = vpiRealVal;
      val.value.real = मूल्य_2;
      vpi_put_मूल्य(handle_, &val, 0, vpiNoDelay);
}

शून्य schedule_evctl(__vpiHandle*handle, द्विगुण value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      प्रतिज्ञा(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = नूतन evctl_real(handle, value, ecount);
      ep->last = &((*(ep->last))->next);
}

evctl_vector::evctl_vector(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&value,
                           unsigned off, unsigned wid, uint64_t ecount)
:evctl(ecount), ptr_(ptr), मूल्य_2(value)
{
      off_ = off;
      wid_ = wid;
}

शून्य evctl_vector::run_run()
{
      यदि (wid_ != 0) {
	    vvp_send_vec4_pv(ptr_, मूल्य_2, off_, wid_, 0);
      } अन्यथा {
	    vvp_send_vec4(ptr_, मूल्य_2, 0);
      }
}

शून्य schedule_evctl(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&value,
                    unsigned offset, unsigned wid,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      प्रतिज्ञा(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = नूतन evctl_vector(ptr, value, offset, wid, ecount);
      ep->last = &((*(ep->last))->next);
}

evctl_array::evctl_array(vvp_array_t memory, unsigned index,
                         ध्रुव vvp_vector4_t&value, unsigned off,
                         uint64_t ecount)
:evctl(ecount), मूल्य_2(value)
{
      mem_ = memory;
      idx_ = index;
      off_ = off;
}

शून्य evctl_array::run_run()
{
      mem_->set_word(idx_, off_, मूल्य_2);
}

शून्य schedule_evctl(vvp_array_t memory, unsigned index,
                    ध्रुव vvp_vector4_t&value, unsigned offset,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      प्रतिज्ञा(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = नूतन evctl_array(memory, index, value, offset, ecount);
      ep->last = &((*(ep->last))->next);
}

evctl_array_r::evctl_array_r(vvp_array_t memory, unsigned index,
                             द्विगुण value, uint64_t ecount)
:evctl(ecount)
{
      mem_ = memory;
      idx_ = index;
      मूल्य_2 = value;
}

शून्य evctl_array_r::run_run()
{
      mem_->set_word(idx_, मूल्य_2);
}

शून्य schedule_evctl(vvp_array_t memory, unsigned index,
                    द्विगुण value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      प्रतिज्ञा(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = नूतन evctl_array_r(memory, index, value, ecount);
      ep->last = &((*(ep->last))->next);
}

अन्तःस्थ vvp_fun_edge::edge_t VVP_EDGE(vvp_bit4_t from, vvp_bit4_t to)
{
      प्रत्यागम 1 << ((from << 2) | to);
}

ध्रुव vvp_fun_edge::edge_t vvp_edge_posedge
      = VVP_EDGE(BIT4_0,BIT4_1)
      | VVP_EDGE(BIT4_0,BIT4_X)
      | VVP_EDGE(BIT4_0,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_1)
      | VVP_EDGE(BIT4_Z,BIT4_1)
      ;

ध्रुव vvp_fun_edge::edge_t vvp_edge_negedge
      = VVP_EDGE(BIT4_1,BIT4_0)
      | VVP_EDGE(BIT4_1,BIT4_X)
      | VVP_EDGE(BIT4_1,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_0)
      | VVP_EDGE(BIT4_Z,BIT4_0)
      ;

ध्रुव vvp_fun_edge::edge_t vvp_edge_edge
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

ध्रुव vvp_fun_edge::edge_t vvp_edge_none    = 0;

संरचना vvp_fun_edge_state_s : सार्वजनिक waitable_state_s {
      vvp_fun_edge_state_s()
      {
            प्रति (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
                  bits[idx] = BIT4_X;
      }

      vvp_bit4_t bits[4];
};

vvp_fun_edge::vvp_fun_edge(edge_t e)
: edge_(e)
{
      प्रति (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
            bits_[idx] = BIT4_X;
}

vvp_fun_edge::~vvp_fun_edge()
{
}

तार्किक vvp_fun_edge::recv_vec4_(ध्रुव vvp_vector4_t&bit,
                              vvp_bit4_t&old_bit, vthread_t&threads)
{
	/* See what kind of edge this represents. */
      edge_t mask = VVP_EDGE(old_bit, bit.value(0));

	/* Save the current input for the next time around. */
      old_bit = bit.value(0);

      यदि ((edge_ == vvp_edge_none) || (edge_ & mask)) {
	    run_waiting_threads_(threads);
            प्रत्यागम सत्य;
      }
      प्रत्यागम असत्य;
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

      प्रत्यागम tmp;
}

शून्य vvp_fun_edge_sa::recv_vec4(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                                vvp_context_t)
{
      यदि (recv_vec4_(bit, bits_[port.port()], threads_)) {
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

शून्य vvp_fun_edge_sa::recv_vec4_pv(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t)
{
      प्रतिज्ञा(base == 0);
      यदि (recv_vec4_(bit, bits_[port.port()], threads_)) {
	    vvp_net_t*net = port.ptr();
	    net->send_vec4_pv(bit, base, vwid, 0);
      }
}

vvp_fun_edge_aa::vvp_fun_edge_aa(edge_t e)
: vvp_fun_edge(e)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(एतत्, context_scope_);
}

vvp_fun_edge_aa::~vvp_fun_edge_aa()
{
}

शून्य vvp_fun_edge_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, नूतन vvp_fun_edge_state_s);
      reset_instance(context);
}

शून्य vvp_fun_edge_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_edge_state_s*state = static_cast<vvp_fun_edge_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
      प्रति (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
            state->bits[idx] = bits_[idx];
}

#ifdef CHECK_WITH_VALGRIND
शून्य vvp_fun_edge_aa::free_instance(vvp_context_t context)
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

      प्रत्यागम tmp;
}

शून्य vvp_fun_edge_aa::recv_vec4(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                                vvp_context_t context)
{
      यदि (context) {
            vvp_fun_edge_state_s*state = static_cast<vvp_fun_edge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            यदि (recv_vec4_(bit, state->bits[port.port()], state->threads)) {
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(bit, context);
            }
      } अन्यथा {
            context = context_scope_->live_contexts;
            यावत् (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            bits_[port.port()] = bit.value(0);
      }
}

वर्ग anyedge_मूल्य {

    सार्वजनिक:
      anyedge_मूल्य() {};
      आभास ~anyedge_मूल्य() {};

      आभास शून्य reset() = 0;

      आभास शून्य duplicate(anyedge_मूल्य*&dup) = 0;
};

वर्ग anyedge_vec4_मूल्य : सार्वजनिक anyedge_मूल्य {

    सार्वजनिक:
      anyedge_vec4_मूल्य() {};
      आभास ~anyedge_vec4_मूल्य() override {};

      शून्य reset() override { old_bits.set_to_x(); }

      शून्य set(ध्रुव vvp_vector4_t&bit) { old_bits = bit; };

      शून्य duplicate(anyedge_मूल्य*&dup) override;

      तार्किक recv_vec4(ध्रुव vvp_vector4_t&bit);

      तार्किक recv_vec4_pv(ध्रुव vvp_vector4_t&bit, unsigned base,
			unsigned vwid);

    गुप्त:
      vvp_vector4_t old_bits;
};

स्थिर anyedge_vec4_मूल्य*get_vec4_मूल्य(anyedge_मूल्य*&value)
{
      anyedge_vec4_मूल्य*vec4_मूल्य = dynamic_cast<anyedge_vec4_मूल्य*>(value);
      यदि (!value) {
	    vec4_मूल्य = नूतन anyedge_vec4_मूल्य();
	    delete value;
	    value = vec4_मूल्य;
      }
      प्रत्यागम vec4_मूल्य;
}

वर्ग anyedge_real_मूल्य : सार्वजनिक anyedge_मूल्य {

    सार्वजनिक:
      anyedge_real_मूल्य() : old_bits(0.0) {};
      आभास ~anyedge_real_मूल्य() override {};

      शून्य reset() override { old_bits = 0.0; }

      शून्य set(द्विगुण bit) { old_bits = bit; };

      शून्य duplicate(anyedge_मूल्य*&dup) override;

      तार्किक recv_real(द्विगुण bit);

    गुप्त:
      द्विगुण old_bits;
};

स्थिर anyedge_real_मूल्य*get_real_मूल्य(anyedge_मूल्य*&value)
{
      anyedge_real_मूल्य*real_मूल्य = dynamic_cast<anyedge_real_मूल्य*>(value);
      यदि (!value) {
	    real_मूल्य = नूतन anyedge_real_मूल्य();
	    delete value;
	    value = real_मूल्य;
      }
      प्रत्यागम real_मूल्य;
}

वर्ग anyedge_string_मूल्य : सार्वजनिक anyedge_मूल्य {

    सार्वजनिक:
      anyedge_string_मूल्य() {};
      आभास ~anyedge_string_मूल्य() override {};

      शून्य reset() override { old_bits.clear(); }

      शून्य set(ध्रुव std::string&bit) { old_bits = bit; };

      शून्य duplicate(anyedge_मूल्य*&dup) override;

      तार्किक recv_string(ध्रुव std::string&bit);

    गुप्त:
      std::string old_bits;
};

स्थिर anyedge_string_मूल्य*get_string_मूल्य(anyedge_मूल्य*&value)
{
      anyedge_string_मूल्य*string_मूल्य = dynamic_cast<anyedge_string_मूल्य*>(value);
      यदि (!value) {
	    string_मूल्य = नूतन anyedge_string_मूल्य();
	    delete value;
	    value = string_मूल्य;
      }
      प्रत्यागम string_मूल्य;
}

संरचना vvp_fun_anyedge_state_s : सार्वजनिक waitable_state_s {
      vvp_fun_anyedge_state_s()
      {
            प्रति (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
                  last_मूल्य[idx] = 0;
      }

      ~vvp_fun_anyedge_state_s()
      {
            प्रति (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
                  delete last_मूल्य[idx];
      }

      anyedge_मूल्य *last_मूल्य[4];
      vvp_fun_anyedge_state_s(ध्रुव vvp_fun_anyedge_state_s&) = delete;
      vvp_fun_anyedge_state_s& operator=(ध्रुव vvp_fun_anyedge_state_s&) = delete;
};

vvp_fun_anyedge::vvp_fun_anyedge()
{
      प्रति (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
	    last_मूल्य[idx] = 0;
}

vvp_fun_anyedge::~vvp_fun_anyedge()
{
      प्रति (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
	    delete last_मूल्य[idx];
}

शून्य anyedge_vec4_मूल्य::duplicate(anyedge_मूल्य*&dup)
{
      anyedge_vec4_मूल्य*dup_vec4 = get_vec4_मूल्य(dup);
      प्रतिज्ञा(dup_vec4);
      dup_vec4->set(old_bits);
}

तार्किक anyedge_vec4_मूल्य::recv_vec4(ध्रुव vvp_vector4_t&bit)
{
      तार्किक flag = असत्य;

      यदि (old_bits.size() != bit.size()) {
	    यदि (old_bits.size() == 0) {
		    // Special case: If we've not seen any input yet
		    // (old_bits.size()==0) then replace it will a reference
		    // vector that is 'bx. Then compare that with the input
		    // to see if we are processing a change from 'bx.
		  old_bits = vvp_vector4_t(bit.size(), BIT4_X);
		  यदि (old_bits.eeq(bit))
			flag = असत्य;
		  अन्यथा
			flag = सत्य;

	    } अन्यथा {
		  flag = सत्य;
	    }

      } अन्यथा {
	    प्रति (unsigned idx = 0 ;  idx < bit.size() ;  idx += 1) {
		  यदि (old_bits.value(idx) != bit.value(idx)) {
			flag = सत्य;
			भङ्ग;
		  }
	    }
      }

      यदि (flag) {
	    old_bits = bit;
      }

      प्रत्यागम flag;
}

तार्किक anyedge_vec4_मूल्य::recv_vec4_pv(ध्रुव vvp_vector4_t&bit, unsigned base,
				      unsigned vwid)
{
      vvp_vector4_t tmp = old_bits;
      यदि (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid, BIT4_Z);
      प्रतिज्ञा(base + bit.size()<= vwid);
      प्रतिज्ञा(tmp.size() == vwid);
      tmp.set_vec(base, bit);

      प्रत्यागम recv_vec4(tmp);
}

शून्य anyedge_real_मूल्य::duplicate(anyedge_मूल्य*&dup)
{
      anyedge_real_मूल्य*dup_real = get_real_मूल्य(dup);
      प्रतिज्ञा(dup_real);
      dup_real->set(old_bits);
}

तार्किक anyedge_real_मूल्य::recv_real(द्विगुण bit)
{
      यदि (old_bits != bit) {
	    old_bits = bit;
            प्रत्यागम सत्य;
      }
      प्रत्यागम असत्य;
}

शून्य anyedge_string_मूल्य::duplicate(anyedge_मूल्य*&dup)
{
      anyedge_string_मूल्य*dup_string = get_string_मूल्य(dup);
      प्रतिज्ञा(dup_string);
      dup_string->set(old_bits);
}

तार्किक anyedge_string_मूल्य::recv_string(ध्रुव std::string&bit)
{
      यदि (old_bits != bit) {
	    old_bits = bit;
            प्रत्यागम सत्य;
      }
      प्रत्यागम असत्य;
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

      प्रत्यागम tmp;
}

शून्य vvp_fun_anyedge_sa::recv_vec4(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                                   vvp_context_t)
{
      anyedge_vec4_मूल्य*value = get_vec4_मूल्य(last_मूल्य[port.port()]);
      प्रतिज्ञा(value);
      यदि (value->recv_vec4(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

शून्य vvp_fun_anyedge_sa::recv_vec4_pv(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
				      unsigned base, unsigned vwid, vvp_context_t)
{
      anyedge_vec4_मूल्य*value = get_vec4_मूल्य(last_मूल्य[port.port()]);
      प्रतिज्ञा(value);
      यदि (value->recv_vec4_pv(bit, base, vwid)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

शून्य vvp_fun_anyedge_sa::recv_real(vvp_net_ptr_t port, द्विगुण bit,
                                   vvp_context_t)
{
      anyedge_real_मूल्य*value = get_real_मूल्य(last_मूल्य[port.port()]);
      प्रतिज्ञा(value);
      यदि (value->recv_real(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

शून्य vvp_fun_anyedge_sa::recv_string(vvp_net_ptr_t port, ध्रुव std::string&bit,
				     vvp_context_t)
{
      anyedge_string_मूल्य*value = get_string_मूल्य(last_मूल्य[port.port()]);
      प्रतिज्ञा(value);
      यदि (value->recv_string(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

/*
 * An anyedge receiving an object should do nothing with it, but should
 * trigger waiting threads.
 */
शून्य vvp_fun_anyedge_sa::recv_object(vvp_net_ptr_t port, vvp_object_t,
				     vvp_context_t)
{
      run_waiting_threads_(threads_);
      vvp_net_t*net = port.ptr();
      net->send_vec4(vvp_vector4_t(), 0);
}

vvp_fun_anyedge_aa::vvp_fun_anyedge_aa()
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(एतत्, context_scope_);
}

vvp_fun_anyedge_aa::~vvp_fun_anyedge_aa()
{
}

शून्य vvp_fun_anyedge_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, नूतन vvp_fun_anyedge_state_s);
      reset_instance(context);
}

शून्य vvp_fun_anyedge_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
      प्रति (unsigned idx = 0 ;  idx < 4 ;  idx += 1) {
	    यदि (last_मूल्य[idx])
	          last_मूल्य[idx]->duplicate(state->last_मूल्य[idx]);
	    अन्यथा यदि (state->last_मूल्य[idx])
		  state->last_मूल्य[idx]->reset();
      }
}

#ifdef CHECK_WITH_VALGRIND
शून्य vvp_fun_anyedge_aa::free_instance(vvp_context_t context)
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

      प्रत्यागम tmp;
}

शून्य vvp_fun_anyedge_aa::recv_vec4(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      यदि (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_vec4_मूल्य*value = get_vec4_मूल्य(state->last_मूल्य[port.port()]);
            प्रतिज्ञा(value);
            यदि (value->recv_vec4(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(bit, context);
            }
      } अन्यथा {
            context = context_scope_->live_contexts;
            यावत् (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            anyedge_vec4_मूल्य*value = get_vec4_मूल्य(last_मूल्य[port.port()]);
            प्रतिज्ञा(value);
            value->set(bit);
      }
}

शून्य vvp_fun_anyedge_aa::recv_real(vvp_net_ptr_t port, द्विगुण bit,
                                   vvp_context_t context)
{
      यदि (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_real_मूल्य*value = get_real_मूल्य(state->last_मूल्य[port.port()]);
            प्रतिज्ञा(value);
            यदि (value->recv_real(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } अन्यथा {
            context = context_scope_->live_contexts;
            यावत् (context) {
                  recv_real(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            anyedge_real_मूल्य*value = get_real_मूल्य(last_मूल्य[port.port()]);
            प्रतिज्ञा(value);
            value->set(bit);
      }
}

शून्य vvp_fun_anyedge_aa::recv_string(vvp_net_ptr_t port, ध्रुव std::string&bit,
				     vvp_context_t context)
{
      यदि (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_string_मूल्य*value = get_string_मूल्य(state->last_मूल्य[port.port()]);
            प्रतिज्ञा(value);
            यदि (value->recv_string(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } अन्यथा {
            context = context_scope_->live_contexts;
            यावत् (context) {
                  recv_string(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            anyedge_string_मूल्य*value = get_string_मूल्य(last_मूल्य[port.port()]);
            प्रतिज्ञा(value);
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

      प्रत्यागम tmp;
}

शून्य vvp_fun_event_or_sa::recv_vec4(vvp_net_ptr_t, ध्रुव vvp_vector4_t&bit,
                                    vvp_context_t)
{
      run_waiting_threads_(threads_);
      base_net_->send_vec4(bit, 0);
}

vvp_fun_event_or_aa::vvp_fun_event_or_aa(vvp_net_t*base_net)
: vvp_fun_event_or(base_net)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(एतत्, context_scope_);
}

vvp_fun_event_or_aa::~vvp_fun_event_or_aa()
{
}

शून्य vvp_fun_event_or_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, नूतन waitable_state_s);
}

शून्य vvp_fun_event_or_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
}

#ifdef CHECK_WITH_VALGRIND
शून्य vvp_fun_event_or_aa::free_instance(vvp_context_t context)
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

      प्रत्यागम tmp;
}

शून्य vvp_fun_event_or_aa::recv_vec4(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                                    vvp_context_t context)
{
      यदि (context) {
            waitable_state_s*state = static_cast<waitable_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            run_waiting_threads_(state->threads);
            base_net_->send_vec4(bit, context);
      } अन्यथा {
            context = context_scope_->live_contexts;
            यावत् (context) {
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

      प्रत्यागम tmp;
}

शून्य vvp_named_event_sa::recv_vec4(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                                   vvp_context_t)
{
      run_waiting_threads_(threads_);
      vvp_net_t*net = port.ptr();
      net->send_vec4(bit, 0);

      __vpiNamedEvent*obj = dynamic_cast<__vpiNamedEvent*>(handle_);
      प्रतिज्ञा(obj);
      obj->run_vpi_callbacks();
}

vvp_named_event_aa::vvp_named_event_aa(__vpiHandle*h)
: vvp_named_event(h)
{
      context_idx_ = vpip_add_item_to_context(एतत्, vpip_peek_context_scope());
}

vvp_named_event_aa::~vvp_named_event_aa()
{
}

शून्य vvp_named_event_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, नूतन waitable_state_s);
}

शून्य vvp_named_event_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
}

#ifdef CHECK_WITH_VALGRIND
शून्य vvp_named_event_aa::free_instance(vvp_context_t context)
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

      प्रत्यागम tmp;
}

शून्य vvp_named_event_aa::recv_vec4(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      प्रतिज्ञा(context);

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

स्थिर शून्य compile_event_or(वर्ण*label, unsigned argc, संरचना symb_s*argv);

शून्य compile_event(वर्ण*label, वर्ण*type, unsigned argc, संरचना symb_s*argv)
{
      vvp_net_fun_t*fun = 0;

      यदि (type == 0) {
	    compile_event_or(label, argc, argv);
	    प्रत्यागम;
      }

      यदि (strcmp(type,"anyedge") == 0) {

	    मुक्त(type);

            यदि (vpip_peek_current_scope()->is_automatic()) {
                  fun = नूतन vvp_fun_anyedge_aa;
            } अन्यथा {
                  fun = नूतन vvp_fun_anyedge_sa;
            }

      } अन्यथा {

	    vvp_fun_edge::edge_t edge_type = vvp_edge_none;

	    यदि (strcmp(type,"posedge") == 0)
		  edge_type = vvp_edge_posedge;
	    अन्यथा यदि (strcmp(type,"negedge") == 0)
		  edge_type = vvp_edge_negedge;
	    अन्यथा यदि (strcmp(type,"edge") == 0)
		  edge_type = vvp_edge_edge;

	    प्रतिज्ञा(argc <= 4);
	    मुक्त(type);

            यदि (vpip_peek_current_scope()->is_automatic()) {
                  fun = नूतन vvp_fun_edge_aa(edge_type);
            } अन्यथा {
                  fun = नूतन vvp_fun_edge_sa(edge_type);
            }

      }

      vvp_net_t* ptr = नूतन vvp_net_t;
      ptr->fun = fun;

      define_functor_चिन्ह(label, ptr);
      मुक्त(label);

      inputs_connect(ptr, argc, argv);
      मुक्त(argv);
}

स्थिर शून्य compile_event_or(वर्ण*label, unsigned argc, संरचना symb_s*argv)
{
      vvp_net_t*base_net = नूतन vvp_net_t;
      यदि (vpip_peek_current_scope()->is_automatic()) {
            base_net->fun = नूतन vvp_fun_event_or_aa(base_net);
      } अन्यथा {
            base_net->fun = नूतन vvp_fun_event_or_sa(base_net);
      }
      define_functor_चिन्ह(label, base_net);
      मुक्त(label);

	/* This is a simplified version of a wide functor. We don't
	   care about the data values or what port they arrived on,
	   so we can use a single shared functor. */
      vvp_net_t*curr_net = base_net;
      प्रति (unsigned idx = 0 ;  idx < argc ;  idx += 1) {
	    यदि (idx > 0 && (idx % 4) == 0) {
		  curr_net = नूतन vvp_net_t;
		  curr_net->fun = base_net->fun;
	    }
	    आदान_connect(curr_net, idx % 4, argv[idx].text);
      }
      मुक्त(argv);
}

/*
 * This handles the compile of named events. This functor has no
 * inputs, it is only accessed by behavioral trigger statements, which
 * in vvp are %set instructions.
 */
शून्य compile_named_event(वर्ण*label, वर्ण*name, तार्किक local_flag)
{
      vvp_net_t*ptr = नूतन vvp_net_t;

      vpiHandle obj = vpip_make_named_event(name, ptr);

      यदि (vpip_peek_current_scope()->is_automatic()) {
            ptr->fun = नूतन vvp_named_event_aa(obj);
      } अन्यथा {
            ptr->fun = नूतन vvp_named_event_sa(obj);
      }
      define_functor_चिन्ह(label, ptr);
      compile_vpi_चिन्ह(label, obj);
      यदि (! local_flag) vpip_attach_to_current_scope(obj);

      मुक्त(label);
      delete[] name;
}

#ifdef CHECK_WITH_VALGRIND
शून्य named_event_delete(__vpiHandle*handle)
{
      delete dynamic_cast<__vpiNamedEvent *>(handle);
}
#endif
