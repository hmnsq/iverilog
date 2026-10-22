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

શૂન્ય waitable_hooks_s::run_waiting_threads_(vthread_t&threads)
{
	// Run the non-blocking event controls.
      last = &event_ctls;
      માટે (evctl*cur = event_ctls; cur != 0;) {
	    જો (cur->dec_and_run()) {
		  evctl*nxt = cur->next;
		  delete cur;
		  cur = nxt;
		  *last = cur;
	    } નહીંતર {
		  last = &(cur->next);
		  cur = cur->next;
	    }
      }

      vthread_t tmp = threads;
      જો (tmp == 0) પાછા;
      threads = 0;

      vthread_schedule_list(tmp);
}

evctl::evctl(uint64_t ecount)
{
      ecount_ = ecount;
      next = 0;
}

તાર્કિક evctl::dec_and_run()
{
      પ્રતિજ્ઞા(ecount_ != 0);

      ecount_ -= 1;
      જો (ecount_ == 0) run_run();

      પાછા ecount_ == 0;
}

evctl_real::evctl_real(__vpiHandle*handle, દ્વિગુણ value,
                       uint64_t ecount)
:evctl(ecount)
{
      handle_ = handle;
      કિંમત_2 = value;
}

શૂન્ય evctl_real::run_run()
{
      t_vpi_કિંમત val;

      val.format = vpiRealVal;
      val.value.real = કિંમત_2;
      vpi_put_કિંમત(handle_, &val, 0, vpiNoDelay);
}

શૂન્ય schedule_evctl(__vpiHandle*handle, દ્વિગુણ value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      પ્રતિજ્ઞા(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = નવું evctl_real(handle, value, ecount);
      ep->last = &((*(ep->last))->next);
}

evctl_vector::evctl_vector(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&value,
                           unsigned off, unsigned wid, uint64_t ecount)
:evctl(ecount), ptr_(ptr), કિંમત_2(value)
{
      off_ = off;
      wid_ = wid;
}

શૂન્ય evctl_vector::run_run()
{
      જો (wid_ != 0) {
	    vvp_send_vec4_pv(ptr_, કિંમત_2, off_, wid_, 0);
      } નહીંતર {
	    vvp_send_vec4(ptr_, કિંમત_2, 0);
      }
}

શૂન્ય schedule_evctl(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&value,
                    unsigned offset, unsigned wid,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      પ્રતિજ્ઞા(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = નવું evctl_vector(ptr, value, offset, wid, ecount);
      ep->last = &((*(ep->last))->next);
}

evctl_array::evctl_array(vvp_array_t memory, unsigned index,
                         અચલ vvp_vector4_t&value, unsigned off,
                         uint64_t ecount)
:evctl(ecount), કિંમત_2(value)
{
      mem_ = memory;
      idx_ = index;
      off_ = off;
}

શૂન્ય evctl_array::run_run()
{
      mem_->set_word(idx_, off_, કિંમત_2);
}

શૂન્ય schedule_evctl(vvp_array_t memory, unsigned index,
                    અચલ vvp_vector4_t&value, unsigned offset,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      પ્રતિજ્ઞા(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = નવું evctl_array(memory, index, value, offset, ecount);
      ep->last = &((*(ep->last))->next);
}

evctl_array_r::evctl_array_r(vvp_array_t memory, unsigned index,
                             દ્વિગુણ value, uint64_t ecount)
:evctl(ecount)
{
      mem_ = memory;
      idx_ = index;
      કિંમત_2 = value;
}

શૂન્ય evctl_array_r::run_run()
{
      mem_->set_word(idx_, કિંમત_2);
}

શૂન્ય schedule_evctl(vvp_array_t memory, unsigned index,
                    દ્વિગુણ value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      પ્રતિજ્ઞા(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = નવું evctl_array_r(memory, index, value, ecount);
      ep->last = &((*(ep->last))->next);
}

અંતઃરેખ vvp_fun_edge::edge_t VVP_EDGE(vvp_bit4_t from, vvp_bit4_t to)
{
      પાછા 1 << ((from << 2) | to);
}

અચલ vvp_fun_edge::edge_t vvp_edge_posedge
      = VVP_EDGE(BIT4_0,BIT4_1)
      | VVP_EDGE(BIT4_0,BIT4_X)
      | VVP_EDGE(BIT4_0,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_1)
      | VVP_EDGE(BIT4_Z,BIT4_1)
      ;

અચલ vvp_fun_edge::edge_t vvp_edge_negedge
      = VVP_EDGE(BIT4_1,BIT4_0)
      | VVP_EDGE(BIT4_1,BIT4_X)
      | VVP_EDGE(BIT4_1,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_0)
      | VVP_EDGE(BIT4_Z,BIT4_0)
      ;

અચલ vvp_fun_edge::edge_t vvp_edge_edge
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

અચલ vvp_fun_edge::edge_t vvp_edge_none    = 0;

સંરચના vvp_fun_edge_state_s : જાહેર waitable_state_s {
      vvp_fun_edge_state_s()
      {
            માટે (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
                  bits[idx] = BIT4_X;
      }

      vvp_bit4_t bits[4];
};

vvp_fun_edge::vvp_fun_edge(edge_t e)
: edge_(e)
{
      માટે (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
            bits_[idx] = BIT4_X;
}

vvp_fun_edge::~vvp_fun_edge()
{
}

તાર્કિક vvp_fun_edge::recv_vec4_(અચલ vvp_vector4_t&bit,
                              vvp_bit4_t&old_bit, vthread_t&threads)
{
	/* See what kind of edge this represents. */
      edge_t mask = VVP_EDGE(old_bit, bit.value(0));

	/* Save the current input for the next time around. */
      old_bit = bit.value(0);

      જો ((edge_ == vvp_edge_none) || (edge_ & mask)) {
	    run_waiting_threads_(threads);
            પાછા સાચું;
      }
      પાછા ખોટું;
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

      પાછા tmp;
}

શૂન્ય vvp_fun_edge_sa::recv_vec4(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
                                vvp_context_t)
{
      જો (recv_vec4_(bit, bits_[port.port()], threads_)) {
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

શૂન્ય vvp_fun_edge_sa::recv_vec4_pv(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t)
{
      પ્રતિજ્ઞા(base == 0);
      જો (recv_vec4_(bit, bits_[port.port()], threads_)) {
	    vvp_net_t*net = port.ptr();
	    net->send_vec4_pv(bit, base, vwid, 0);
      }
}

vvp_fun_edge_aa::vvp_fun_edge_aa(edge_t e)
: vvp_fun_edge(e)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(આ, context_scope_);
}

vvp_fun_edge_aa::~vvp_fun_edge_aa()
{
}

શૂન્ય vvp_fun_edge_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, નવું vvp_fun_edge_state_s);
      reset_instance(context);
}

શૂન્ય vvp_fun_edge_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_edge_state_s*state = static_cast<vvp_fun_edge_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
      માટે (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
            state->bits[idx] = bits_[idx];
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય vvp_fun_edge_aa::free_instance(vvp_context_t context)
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

      પાછા tmp;
}

શૂન્ય vvp_fun_edge_aa::recv_vec4(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
                                vvp_context_t context)
{
      જો (context) {
            vvp_fun_edge_state_s*state = static_cast<vvp_fun_edge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            જો (recv_vec4_(bit, state->bits[port.port()], state->threads)) {
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(bit, context);
            }
      } નહીંતર {
            context = context_scope_->live_contexts;
            જ્યારે (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            bits_[port.port()] = bit.value(0);
      }
}

વર્ગ anyedge_કિંમત {

    જાહેર:
      anyedge_કિંમત() {};
      આભાસી ~anyedge_કિંમત() {};

      આભાસી શૂન્ય reset() = 0;

      આભાસી શૂન્ય duplicate(anyedge_કિંમત*&dup) = 0;
};

વર્ગ anyedge_vec4_કિંમત : જાહેર anyedge_કિંમત {

    જાહેર:
      anyedge_vec4_કિંમત() {};
      આભાસી ~anyedge_vec4_કિંમત() override {};

      શૂન્ય reset() override { old_bits.set_to_x(); }

      શૂન્ય set(અચલ vvp_vector4_t&bit) { old_bits = bit; };

      શૂન્ય duplicate(anyedge_કિંમત*&dup) override;

      તાર્કિક recv_vec4(અચલ vvp_vector4_t&bit);

      તાર્કિક recv_vec4_pv(અચલ vvp_vector4_t&bit, unsigned base,
			unsigned vwid);

    ખાનગી:
      vvp_vector4_t old_bits;
};

સ્થિર anyedge_vec4_કિંમત*get_vec4_કિંમત(anyedge_કિંમત*&value)
{
      anyedge_vec4_કિંમત*vec4_કિંમત = dynamic_cast<anyedge_vec4_કિંમત*>(value);
      જો (!value) {
	    vec4_કિંમત = નવું anyedge_vec4_કિંમત();
	    delete value;
	    value = vec4_કિંમત;
      }
      પાછા vec4_કિંમત;
}

વર્ગ anyedge_real_કિંમત : જાહેર anyedge_કિંમત {

    જાહેર:
      anyedge_real_કિંમત() : old_bits(0.0) {};
      આભાસી ~anyedge_real_કિંમત() override {};

      શૂન્ય reset() override { old_bits = 0.0; }

      શૂન્ય set(દ્વિગુણ bit) { old_bits = bit; };

      શૂન્ય duplicate(anyedge_કિંમત*&dup) override;

      તાર્કિક recv_real(દ્વિગુણ bit);

    ખાનગી:
      દ્વિગુણ old_bits;
};

સ્થિર anyedge_real_કિંમત*get_real_કિંમત(anyedge_કિંમત*&value)
{
      anyedge_real_કિંમત*real_કિંમત = dynamic_cast<anyedge_real_કિંમત*>(value);
      જો (!value) {
	    real_કિંમત = નવું anyedge_real_કિંમત();
	    delete value;
	    value = real_કિંમત;
      }
      પાછા real_કિંમત;
}

વર્ગ anyedge_string_કિંમત : જાહેર anyedge_કિંમત {

    જાહેર:
      anyedge_string_કિંમત() {};
      આભાસી ~anyedge_string_કિંમત() override {};

      શૂન્ય reset() override { old_bits.clear(); }

      શૂન્ય set(અચલ std::string&bit) { old_bits = bit; };

      શૂન્ય duplicate(anyedge_કિંમત*&dup) override;

      તાર્કિક recv_string(અચલ std::string&bit);

    ખાનગી:
      std::string old_bits;
};

સ્થિર anyedge_string_કિંમત*get_string_કિંમત(anyedge_કિંમત*&value)
{
      anyedge_string_કિંમત*string_કિંમત = dynamic_cast<anyedge_string_કિંમત*>(value);
      જો (!value) {
	    string_કિંમત = નવું anyedge_string_કિંમત();
	    delete value;
	    value = string_કિંમત;
      }
      પાછા string_કિંમત;
}

સંરચના vvp_fun_anyedge_state_s : જાહેર waitable_state_s {
      vvp_fun_anyedge_state_s()
      {
            માટે (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
                  last_કિંમત[idx] = 0;
      }

      ~vvp_fun_anyedge_state_s()
      {
            માટે (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
                  delete last_કિંમત[idx];
      }

      anyedge_કિંમત *last_કિંમત[4];
      vvp_fun_anyedge_state_s(અચલ vvp_fun_anyedge_state_s&) = delete;
      vvp_fun_anyedge_state_s& operator=(અચલ vvp_fun_anyedge_state_s&) = delete;
};

vvp_fun_anyedge::vvp_fun_anyedge()
{
      માટે (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
	    last_કિંમત[idx] = 0;
}

vvp_fun_anyedge::~vvp_fun_anyedge()
{
      માટે (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
	    delete last_કિંમત[idx];
}

શૂન્ય anyedge_vec4_કિંમત::duplicate(anyedge_કિંમત*&dup)
{
      anyedge_vec4_કિંમત*dup_vec4 = get_vec4_કિંમત(dup);
      પ્રતિજ્ઞા(dup_vec4);
      dup_vec4->set(old_bits);
}

તાર્કિક anyedge_vec4_કિંમત::recv_vec4(અચલ vvp_vector4_t&bit)
{
      તાર્કિક flag = ખોટું;

      જો (old_bits.size() != bit.size()) {
	    જો (old_bits.size() == 0) {
		    // Special case: If we've not seen any input yet
		    // (old_bits.size()==0) then replace it will a reference
		    // vector that is 'bx. Then compare that with the input
		    // to see if we are processing a change from 'bx.
		  old_bits = vvp_vector4_t(bit.size(), BIT4_X);
		  જો (old_bits.eeq(bit))
			flag = ખોટું;
		  નહીંતર
			flag = સાચું;

	    } નહીંતર {
		  flag = સાચું;
	    }

      } નહીંતર {
	    માટે (unsigned idx = 0 ;  idx < bit.size() ;  idx += 1) {
		  જો (old_bits.value(idx) != bit.value(idx)) {
			flag = સાચું;
			તોડો;
		  }
	    }
      }

      જો (flag) {
	    old_bits = bit;
      }

      પાછા flag;
}

તાર્કિક anyedge_vec4_કિંમત::recv_vec4_pv(અચલ vvp_vector4_t&bit, unsigned base,
				      unsigned vwid)
{
      vvp_vector4_t tmp = old_bits;
      જો (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid, BIT4_Z);
      પ્રતિજ્ઞા(base + bit.size()<= vwid);
      પ્રતિજ્ઞા(tmp.size() == vwid);
      tmp.set_vec(base, bit);

      પાછા recv_vec4(tmp);
}

શૂન્ય anyedge_real_કિંમત::duplicate(anyedge_કિંમત*&dup)
{
      anyedge_real_કિંમત*dup_real = get_real_કિંમત(dup);
      પ્રતિજ્ઞા(dup_real);
      dup_real->set(old_bits);
}

તાર્કિક anyedge_real_કિંમત::recv_real(દ્વિગુણ bit)
{
      જો (old_bits != bit) {
	    old_bits = bit;
            પાછા સાચું;
      }
      પાછા ખોટું;
}

શૂન્ય anyedge_string_કિંમત::duplicate(anyedge_કિંમત*&dup)
{
      anyedge_string_કિંમત*dup_string = get_string_કિંમત(dup);
      પ્રતિજ્ઞા(dup_string);
      dup_string->set(old_bits);
}

તાર્કિક anyedge_string_કિંમત::recv_string(અચલ std::string&bit)
{
      જો (old_bits != bit) {
	    old_bits = bit;
            પાછા સાચું;
      }
      પાછા ખોટું;
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

      પાછા tmp;
}

શૂન્ય vvp_fun_anyedge_sa::recv_vec4(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
                                   vvp_context_t)
{
      anyedge_vec4_કિંમત*value = get_vec4_કિંમત(last_કિંમત[port.port()]);
      પ્રતિજ્ઞા(value);
      જો (value->recv_vec4(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

શૂન્ય vvp_fun_anyedge_sa::recv_vec4_pv(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
				      unsigned base, unsigned vwid, vvp_context_t)
{
      anyedge_vec4_કિંમત*value = get_vec4_કિંમત(last_કિંમત[port.port()]);
      પ્રતિજ્ઞા(value);
      જો (value->recv_vec4_pv(bit, base, vwid)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

શૂન્ય vvp_fun_anyedge_sa::recv_real(vvp_net_ptr_t port, દ્વિગુણ bit,
                                   vvp_context_t)
{
      anyedge_real_કિંમત*value = get_real_કિંમત(last_કિંમત[port.port()]);
      પ્રતિજ્ઞા(value);
      જો (value->recv_real(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

શૂન્ય vvp_fun_anyedge_sa::recv_string(vvp_net_ptr_t port, અચલ std::string&bit,
				     vvp_context_t)
{
      anyedge_string_કિંમત*value = get_string_કિંમત(last_કિંમત[port.port()]);
      પ્રતિજ્ઞા(value);
      જો (value->recv_string(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

/*
 * An anyedge receiving an object should do nothing with it, but should
 * trigger waiting threads.
 */
શૂન્ય vvp_fun_anyedge_sa::recv_object(vvp_net_ptr_t port, vvp_object_t,
				     vvp_context_t)
{
      run_waiting_threads_(threads_);
      vvp_net_t*net = port.ptr();
      net->send_vec4(vvp_vector4_t(), 0);
}

vvp_fun_anyedge_aa::vvp_fun_anyedge_aa()
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(આ, context_scope_);
}

vvp_fun_anyedge_aa::~vvp_fun_anyedge_aa()
{
}

શૂન્ય vvp_fun_anyedge_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, નવું vvp_fun_anyedge_state_s);
      reset_instance(context);
}

શૂન્ય vvp_fun_anyedge_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
      માટે (unsigned idx = 0 ;  idx < 4 ;  idx += 1) {
	    જો (last_કિંમત[idx])
	          last_કિંમત[idx]->duplicate(state->last_કિંમત[idx]);
	    નહીંતર જો (state->last_કિંમત[idx])
		  state->last_કિંમત[idx]->reset();
      }
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય vvp_fun_anyedge_aa::free_instance(vvp_context_t context)
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

      પાછા tmp;
}

શૂન્ય vvp_fun_anyedge_aa::recv_vec4(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      જો (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_vec4_કિંમત*value = get_vec4_કિંમત(state->last_કિંમત[port.port()]);
            પ્રતિજ્ઞા(value);
            જો (value->recv_vec4(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(bit, context);
            }
      } નહીંતર {
            context = context_scope_->live_contexts;
            જ્યારે (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            anyedge_vec4_કિંમત*value = get_vec4_કિંમત(last_કિંમત[port.port()]);
            પ્રતિજ્ઞા(value);
            value->set(bit);
      }
}

શૂન્ય vvp_fun_anyedge_aa::recv_real(vvp_net_ptr_t port, દ્વિગુણ bit,
                                   vvp_context_t context)
{
      જો (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_real_કિંમત*value = get_real_કિંમત(state->last_કિંમત[port.port()]);
            પ્રતિજ્ઞા(value);
            જો (value->recv_real(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } નહીંતર {
            context = context_scope_->live_contexts;
            જ્યારે (context) {
                  recv_real(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            anyedge_real_કિંમત*value = get_real_કિંમત(last_કિંમત[port.port()]);
            પ્રતિજ્ઞા(value);
            value->set(bit);
      }
}

શૂન્ય vvp_fun_anyedge_aa::recv_string(vvp_net_ptr_t port, અચલ std::string&bit,
				     vvp_context_t context)
{
      જો (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_string_કિંમત*value = get_string_કિંમત(state->last_કિંમત[port.port()]);
            પ્રતિજ્ઞા(value);
            જો (value->recv_string(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } નહીંતર {
            context = context_scope_->live_contexts;
            જ્યારે (context) {
                  recv_string(port, bit, context);
                  context = vvp_get_next_context(context);
            }
            anyedge_string_કિંમત*value = get_string_કિંમત(last_કિંમત[port.port()]);
            પ્રતિજ્ઞા(value);
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

      પાછા tmp;
}

શૂન્ય vvp_fun_event_or_sa::recv_vec4(vvp_net_ptr_t, અચલ vvp_vector4_t&bit,
                                    vvp_context_t)
{
      run_waiting_threads_(threads_);
      base_net_->send_vec4(bit, 0);
}

vvp_fun_event_or_aa::vvp_fun_event_or_aa(vvp_net_t*base_net)
: vvp_fun_event_or(base_net)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(આ, context_scope_);
}

vvp_fun_event_or_aa::~vvp_fun_event_or_aa()
{
}

શૂન્ય vvp_fun_event_or_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, નવું waitable_state_s);
}

શૂન્ય vvp_fun_event_or_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય vvp_fun_event_or_aa::free_instance(vvp_context_t context)
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

      પાછા tmp;
}

શૂન્ય vvp_fun_event_or_aa::recv_vec4(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
                                    vvp_context_t context)
{
      જો (context) {
            waitable_state_s*state = static_cast<waitable_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            run_waiting_threads_(state->threads);
            base_net_->send_vec4(bit, context);
      } નહીંતર {
            context = context_scope_->live_contexts;
            જ્યારે (context) {
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

      પાછા tmp;
}

શૂન્ય vvp_named_event_sa::recv_vec4(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
                                   vvp_context_t)
{
      run_waiting_threads_(threads_);
      vvp_net_t*net = port.ptr();
      net->send_vec4(bit, 0);

      __vpiNamedEvent*obj = dynamic_cast<__vpiNamedEvent*>(handle_);
      પ્રતિજ્ઞા(obj);
      obj->run_vpi_callbacks();
}

vvp_named_event_aa::vvp_named_event_aa(__vpiHandle*h)
: vvp_named_event(h)
{
      context_idx_ = vpip_add_item_to_context(આ, vpip_peek_context_scope());
}

vvp_named_event_aa::~vvp_named_event_aa()
{
}

શૂન્ય vvp_named_event_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, નવું waitable_state_s);
}

શૂન્ય vvp_named_event_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય vvp_named_event_aa::free_instance(vvp_context_t context)
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

      પાછા tmp;
}

શૂન્ય vvp_named_event_aa::recv_vec4(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      પ્રતિજ્ઞા(context);

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

સ્થિર શૂન્ય compile_event_or(અક્ષર*label, unsigned argc, સંરચના symb_s*argv);

શૂન્ય compile_event(અક્ષર*label, અક્ષર*type, unsigned argc, સંરચના symb_s*argv)
{
      vvp_net_fun_t*fun = 0;

      જો (type == 0) {
	    compile_event_or(label, argc, argv);
	    પાછા;
      }

      જો (strcmp(type,"anyedge") == 0) {

	    મુક્ત(type);

            જો (vpip_peek_current_scope()->is_automatic()) {
                  fun = નવું vvp_fun_anyedge_aa;
            } નહીંતર {
                  fun = નવું vvp_fun_anyedge_sa;
            }

      } નહીંતર {

	    vvp_fun_edge::edge_t edge_type = vvp_edge_none;

	    જો (strcmp(type,"posedge") == 0)
		  edge_type = vvp_edge_posedge;
	    નહીંતર જો (strcmp(type,"negedge") == 0)
		  edge_type = vvp_edge_negedge;
	    નહીંતર જો (strcmp(type,"edge") == 0)
		  edge_type = vvp_edge_edge;

	    પ્રતિજ્ઞા(argc <= 4);
	    મુક્ત(type);

            જો (vpip_peek_current_scope()->is_automatic()) {
                  fun = નવું vvp_fun_edge_aa(edge_type);
            } નહીંતર {
                  fun = નવું vvp_fun_edge_sa(edge_type);
            }

      }

      vvp_net_t* ptr = નવું vvp_net_t;
      ptr->fun = fun;

      define_functor_પ્રતીક(label, ptr);
      મુક્ત(label);

      inputs_connect(ptr, argc, argv);
      મુક્ત(argv);
}

સ્થિર શૂન્ય compile_event_or(અક્ષર*label, unsigned argc, સંરચના symb_s*argv)
{
      vvp_net_t*base_net = નવું vvp_net_t;
      જો (vpip_peek_current_scope()->is_automatic()) {
            base_net->fun = નવું vvp_fun_event_or_aa(base_net);
      } નહીંતર {
            base_net->fun = નવું vvp_fun_event_or_sa(base_net);
      }
      define_functor_પ્રતીક(label, base_net);
      મુક્ત(label);

	/* This is a simplified version of a wide functor. We don't
	   care about the data values or what port they arrived on,
	   so we can use a single shared functor. */
      vvp_net_t*curr_net = base_net;
      માટે (unsigned idx = 0 ;  idx < argc ;  idx += 1) {
	    જો (idx > 0 && (idx % 4) == 0) {
		  curr_net = નવું vvp_net_t;
		  curr_net->fun = base_net->fun;
	    }
	    આગત_connect(curr_net, idx % 4, argv[idx].text);
      }
      મુક્ત(argv);
}

/*
 * This handles the compile of named events. This functor has no
 * inputs, it is only accessed by behavioral trigger statements, which
 * in vvp are %set instructions.
 */
શૂન્ય compile_named_event(અક્ષર*label, અક્ષર*name, તાર્કિક local_flag)
{
      vvp_net_t*ptr = નવું vvp_net_t;

      vpiHandle obj = vpip_make_named_event(name, ptr);

      જો (vpip_peek_current_scope()->is_automatic()) {
            ptr->fun = નવું vvp_named_event_aa(obj);
      } નહીંતર {
            ptr->fun = નવું vvp_named_event_sa(obj);
      }
      define_functor_પ્રતીક(label, ptr);
      compile_vpi_પ્રતીક(label, obj);
      જો (! local_flag) vpip_attach_to_current_scope(obj);

      મુક્ત(label);
      delete[] name;
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય named_event_delete(__vpiHandle*handle)
{
      delete dynamic_cast<__vpiNamedEvent *>(handle);
}
#endif
