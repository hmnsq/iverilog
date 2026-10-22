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

व्योम waitable_hooks_s::run_waiting_threads_(vthread_t&threads)
{
	// Run the non-blocking event controls.
      last = &event_ctls;
      क्रम (evctl*cur = event_ctls; cur != 0;) {
	    अगर (cur->dec_and_run()) {
		  evctl*nxt = cur->अग्र_2;
		  मिटाओ cur;
		  cur = nxt;
		  *last = cur;
	    } अन्यथा {
		  last = &(cur->अग्र_2);
		  cur = cur->अग्र_2;
	    }
      }

      vthread_t tmp = threads;
      अगर (tmp == 0) वापस;
      threads = 0;

      vthread_schedule_list(tmp);
}

evctl::evctl(uint64_t ecount)
{
      ecount_ = ecount;
      अग्र_2 = 0;
}

bool evctl::dec_and_run()
{
      निश्चित(ecount_ != 0);

      ecount_ -= 1;
      अगर (ecount_ == 0) run_run();

      वापस ecount_ == 0;
}

evctl_real::evctl_real(__vpiHandle*handle, द्विगुन value,
                       uint64_t ecount)
:evctl(ecount)
{
      handle_ = handle;
      मूल्य_2 = value;
}

व्योम evctl_real::run_run()
{
      t_vpi_मूल्य val;

      val.format = vpiRealVal;
      val.value.real = मूल्य_2;
      vpi_put_मूल्य(handle_, &val, 0, vpiNoDelay);
}

व्योम schedule_evctl(__vpiHandle*handle, द्विगुन value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      निश्चित(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = नया evctl_real(handle, value, ecount);
      ep->last = &((*(ep->last))->अग्र_2);
}

evctl_vector::evctl_vector(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&value,
                           अचिन्हित off, अचिन्हित wid, uint64_t ecount)
:evctl(ecount), ptr_(ptr), मूल्य_2(value)
{
      off_ = off;
      wid_ = wid;
}

व्योम evctl_vector::run_run()
{
      अगर (wid_ != 0) {
	    vvp_send_vec4_pv(ptr_, मूल्य_2, off_, wid_, 0);
      } अन्यथा {
	    vvp_send_vec4(ptr_, मूल्य_2, 0);
      }
}

व्योम schedule_evctl(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&value,
                    अचिन्हित offset, अचिन्हित wid,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      निश्चित(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = नया evctl_vector(ptr, value, offset, wid, ecount);
      ep->last = &((*(ep->last))->अग्र_2);
}

evctl_array::evctl_array(vvp_array_t memory, अचिन्हित सूचकांक,
                         स्थिर vvp_vector4_t&value, अचिन्हित off,
                         uint64_t ecount)
:evctl(ecount), मूल्य_2(value)
{
      mem_ = memory;
      idx_ = सूचकांक;
      off_ = off;
}

व्योम evctl_array::run_run()
{
      mem_->set_word(idx_, off_, मूल्य_2);
}

व्योम schedule_evctl(vvp_array_t memory, अचिन्हित सूचकांक,
                    स्थिर vvp_vector4_t&value, अचिन्हित offset,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      निश्चित(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = नया evctl_array(memory, सूचकांक, value, offset, ecount);
      ep->last = &((*(ep->last))->अग्र_2);
}

evctl_array_r::evctl_array_r(vvp_array_t memory, अचिन्हित सूचकांक,
                             द्विगुन value, uint64_t ecount)
:evctl(ecount)
{
      mem_ = memory;
      idx_ = सूचकांक;
      मूल्य_2 = value;
}

व्योम evctl_array_r::run_run()
{
      mem_->set_word(idx_, मूल्य_2);
}

व्योम schedule_evctl(vvp_array_t memory, अचिन्हित सूचकांक,
                    द्विगुन value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      निश्चित(ep);
	// Now add this call to the end of the event list.
      *(ep->last) = नया evctl_array_r(memory, सूचकांक, value, ecount);
      ep->last = &((*(ep->last))->अग्र_2);
}

अंतरभूत vvp_fun_edge::edge_t VVP_EDGE(vvp_bit4_t from, vvp_bit4_t to)
{
      वापस 1 << ((from << 2) | to);
}

स्थिर vvp_fun_edge::edge_t vvp_edge_posedge
      = VVP_EDGE(BIT4_0,BIT4_1)
      | VVP_EDGE(BIT4_0,BIT4_X)
      | VVP_EDGE(BIT4_0,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_1)
      | VVP_EDGE(BIT4_Z,BIT4_1)
      ;

स्थिर vvp_fun_edge::edge_t vvp_edge_negedge
      = VVP_EDGE(BIT4_1,BIT4_0)
      | VVP_EDGE(BIT4_1,BIT4_X)
      | VVP_EDGE(BIT4_1,BIT4_Z)
      | VVP_EDGE(BIT4_X,BIT4_0)
      | VVP_EDGE(BIT4_Z,BIT4_0)
      ;

स्थिर vvp_fun_edge::edge_t vvp_edge_edge
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

स्थिर vvp_fun_edge::edge_t vvp_edge_none    = 0;

काष्ठा vvp_fun_edge_state_s : खुला waitable_state_s {
      vvp_fun_edge_state_s()
      {
            क्रम (अचिन्हित idx = 0 ;  idx < 4 ;  idx += 1)
                  bits[idx] = BIT4_X;
      }

      vvp_bit4_t bits[4];
};

vvp_fun_edge::vvp_fun_edge(edge_t e)
: edge_(e)
{
      क्रम (अचिन्हित idx = 0 ;  idx < 4 ;  idx += 1)
            bits_[idx] = BIT4_X;
}

vvp_fun_edge::~vvp_fun_edge()
{
}

bool vvp_fun_edge::recv_vec4_(स्थिर vvp_vector4_t&bit,
                              vvp_bit4_t&old_bit, vthread_t&threads)
{
	/* See what kind of edge this represents. */
      edge_t mask = VVP_EDGE(old_bit, bit.value(0));

	/* Save the current input for the next time around. */
      old_bit = bit.value(0);

      अगर ((edge_ == vvp_edge_none) || (edge_ & mask)) {
	    run_waiting_threads_(threads);
            वापस true;
      }
      वापस false;
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

      वापस tmp;
}

व्योम vvp_fun_edge_sa::recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                                vvp_context_t)
{
      अगर (recv_vec4_(bit, bits_[port.port()], threads_)) {
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

व्योम vvp_fun_edge_sa::recv_vec4_pv(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
				   अचिन्हित base, अचिन्हित vwid, vvp_context_t)
{
      निश्चित(base == 0);
      अगर (recv_vec4_(bit, bits_[port.port()], threads_)) {
	    vvp_net_t*net = port.ptr();
	    net->send_vec4_pv(bit, base, vwid, 0);
      }
}

vvp_fun_edge_aa::vvp_fun_edge_aa(edge_t e)
: vvp_fun_edge(e)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(यह, context_scope_);
}

vvp_fun_edge_aa::~vvp_fun_edge_aa()
{
}

व्योम vvp_fun_edge_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, नया vvp_fun_edge_state_s);
      reset_instance(context);
}

व्योम vvp_fun_edge_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_edge_state_s*state = static_cast<vvp_fun_edge_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
      क्रम (अचिन्हित idx = 0 ;  idx < 4 ;  idx += 1)
            state->bits[idx] = bits_[idx];
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम vvp_fun_edge_aa::free_instance(vvp_context_t context)
{
      vvp_fun_edge_state_s*state = static_cast<vvp_fun_edge_state_s*>
            (vvp_get_context_item(context, context_idx_));
      मिटाओ state;
}
#पूर्ण_अगर

vthread_t vvp_fun_edge_aa::add_waiting_thread(vthread_t thread)
{
      vvp_fun_edge_state_s*state = static_cast<vvp_fun_edge_state_s*>
            (vthread_get_wt_context_item(context_idx_));

      vthread_t tmp = state->threads;
      state->threads = thread;

      वापस tmp;
}

व्योम vvp_fun_edge_aa::recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                                vvp_context_t context)
{
      अगर (context) {
            vvp_fun_edge_state_s*state = static_cast<vvp_fun_edge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            अगर (recv_vec4_(bit, state->bits[port.port()], state->threads)) {
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(bit, context);
            }
      } अन्यथा {
            context = context_scope_->live_contexts;
            जबतक (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_अग्र_context(context);
            }
            bits_[port.port()] = bit.value(0);
      }
}

श्रेणी anyedge_मूल्य {

    खुला:
      anyedge_मूल्य() {};
      भव ~anyedge_मूल्य() {};

      भव व्योम reset() = 0;

      भव व्योम duplicate(anyedge_मूल्य*&dup) = 0;
};

श्रेणी anyedge_vec4_मूल्य : खुला anyedge_मूल्य {

    खुला:
      anyedge_vec4_मूल्य() {};
      भव ~anyedge_vec4_मूल्य() override {};

      व्योम reset() override { old_bits.set_to_x(); }

      व्योम set(स्थिर vvp_vector4_t&bit) { old_bits = bit; };

      व्योम duplicate(anyedge_मूल्य*&dup) override;

      bool recv_vec4(स्थिर vvp_vector4_t&bit);

      bool recv_vec4_pv(स्थिर vvp_vector4_t&bit, अचिन्हित base,
			अचिन्हित vwid);

    निजी:
      vvp_vector4_t old_bits;
};

अटल anyedge_vec4_मूल्य*get_vec4_मूल्य(anyedge_मूल्य*&value)
{
      anyedge_vec4_मूल्य*vec4_मूल्य = dynamic_cast<anyedge_vec4_मूल्य*>(value);
      अगर (!value) {
	    vec4_मूल्य = नया anyedge_vec4_मूल्य();
	    मिटाओ value;
	    value = vec4_मूल्य;
      }
      वापस vec4_मूल्य;
}

श्रेणी anyedge_real_मूल्य : खुला anyedge_मूल्य {

    खुला:
      anyedge_real_मूल्य() : old_bits(0.0) {};
      भव ~anyedge_real_मूल्य() override {};

      व्योम reset() override { old_bits = 0.0; }

      व्योम set(द्विगुन bit) { old_bits = bit; };

      व्योम duplicate(anyedge_मूल्य*&dup) override;

      bool recv_real(द्विगुन bit);

    निजी:
      द्विगुन old_bits;
};

अटल anyedge_real_मूल्य*get_real_मूल्य(anyedge_मूल्य*&value)
{
      anyedge_real_मूल्य*real_मूल्य = dynamic_cast<anyedge_real_मूल्य*>(value);
      अगर (!value) {
	    real_मूल्य = नया anyedge_real_मूल्य();
	    मिटाओ value;
	    value = real_मूल्य;
      }
      वापस real_मूल्य;
}

श्रेणी anyedge_string_मूल्य : खुला anyedge_मूल्य {

    खुला:
      anyedge_string_मूल्य() {};
      भव ~anyedge_string_मूल्य() override {};

      व्योम reset() override { old_bits.सफा(); }

      व्योम set(स्थिर std::string&bit) { old_bits = bit; };

      व्योम duplicate(anyedge_मूल्य*&dup) override;

      bool recv_string(स्थिर std::string&bit);

    निजी:
      std::string old_bits;
};

अटल anyedge_string_मूल्य*get_string_मूल्य(anyedge_मूल्य*&value)
{
      anyedge_string_मूल्य*string_मूल्य = dynamic_cast<anyedge_string_मूल्य*>(value);
      अगर (!value) {
	    string_मूल्य = नया anyedge_string_मूल्य();
	    मिटाओ value;
	    value = string_मूल्य;
      }
      वापस string_मूल्य;
}

काष्ठा vvp_fun_anyedge_state_s : खुला waitable_state_s {
      vvp_fun_anyedge_state_s()
      {
            क्रम (अचिन्हित idx = 0 ;  idx < 4 ;  idx += 1)
                  last_मूल्य[idx] = 0;
      }

      ~vvp_fun_anyedge_state_s()
      {
            क्रम (अचिन्हित idx = 0 ;  idx < 4 ;  idx += 1)
                  मिटाओ last_मूल्य[idx];
      }

      anyedge_मूल्य *last_मूल्य[4];
      vvp_fun_anyedge_state_s(स्थिर vvp_fun_anyedge_state_s&) = मिटाओ;
      vvp_fun_anyedge_state_s& चालक=(स्थिर vvp_fun_anyedge_state_s&) = मिटाओ;
};

vvp_fun_anyedge::vvp_fun_anyedge()
{
      क्रम (अचिन्हित idx = 0 ;  idx < 4 ;  idx += 1)
	    last_मूल्य[idx] = 0;
}

vvp_fun_anyedge::~vvp_fun_anyedge()
{
      क्रम (अचिन्हित idx = 0 ;  idx < 4 ;  idx += 1)
	    मिटाओ last_मूल्य[idx];
}

व्योम anyedge_vec4_मूल्य::duplicate(anyedge_मूल्य*&dup)
{
      anyedge_vec4_मूल्य*dup_vec4 = get_vec4_मूल्य(dup);
      निश्चित(dup_vec4);
      dup_vec4->set(old_bits);
}

bool anyedge_vec4_मूल्य::recv_vec4(स्थिर vvp_vector4_t&bit)
{
      bool flag = false;

      अगर (old_bits.size() != bit.size()) {
	    अगर (old_bits.size() == 0) {
		    // Special case: If we've not seen any input yet
		    // (old_bits.size()==0) then replace it will a reference
		    // vector that is 'bx. Then compare that with the input
		    // to see if we are processing a change from 'bx.
		  old_bits = vvp_vector4_t(bit.size(), BIT4_X);
		  अगर (old_bits.eeq(bit))
			flag = false;
		  अन्यथा
			flag = true;

	    } अन्यथा {
		  flag = true;
	    }

      } अन्यथा {
	    क्रम (अचिन्हित idx = 0 ;  idx < bit.size() ;  idx += 1) {
		  अगर (old_bits.value(idx) != bit.value(idx)) {
			flag = true;
			अवरोध;
		  }
	    }
      }

      अगर (flag) {
	    old_bits = bit;
      }

      वापस flag;
}

bool anyedge_vec4_मूल्य::recv_vec4_pv(स्थिर vvp_vector4_t&bit, अचिन्हित base,
				      अचिन्हित vwid)
{
      vvp_vector4_t tmp = old_bits;
      अगर (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid, BIT4_Z);
      निश्चित(base + bit.size()<= vwid);
      निश्चित(tmp.size() == vwid);
      tmp.set_vec(base, bit);

      वापस recv_vec4(tmp);
}

व्योम anyedge_real_मूल्य::duplicate(anyedge_मूल्य*&dup)
{
      anyedge_real_मूल्य*dup_real = get_real_मूल्य(dup);
      निश्चित(dup_real);
      dup_real->set(old_bits);
}

bool anyedge_real_मूल्य::recv_real(द्विगुन bit)
{
      अगर (old_bits != bit) {
	    old_bits = bit;
            वापस true;
      }
      वापस false;
}

व्योम anyedge_string_मूल्य::duplicate(anyedge_मूल्य*&dup)
{
      anyedge_string_मूल्य*dup_string = get_string_मूल्य(dup);
      निश्चित(dup_string);
      dup_string->set(old_bits);
}

bool anyedge_string_मूल्य::recv_string(स्थिर std::string&bit)
{
      अगर (old_bits != bit) {
	    old_bits = bit;
            वापस true;
      }
      वापस false;
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

      वापस tmp;
}

व्योम vvp_fun_anyedge_sa::recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                                   vvp_context_t)
{
      anyedge_vec4_मूल्य*value = get_vec4_मूल्य(last_मूल्य[port.port()]);
      निश्चित(value);
      अगर (value->recv_vec4(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

व्योम vvp_fun_anyedge_sa::recv_vec4_pv(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
				      अचिन्हित base, अचिन्हित vwid, vvp_context_t)
{
      anyedge_vec4_मूल्य*value = get_vec4_मूल्य(last_मूल्य[port.port()]);
      निश्चित(value);
      अगर (value->recv_vec4_pv(bit, base, vwid)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(bit, 0);
      }
}

व्योम vvp_fun_anyedge_sa::recv_real(vvp_net_ptr_t port, द्विगुन bit,
                                   vvp_context_t)
{
      anyedge_real_मूल्य*value = get_real_मूल्य(last_मूल्य[port.port()]);
      निश्चित(value);
      अगर (value->recv_real(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

व्योम vvp_fun_anyedge_sa::recv_string(vvp_net_ptr_t port, स्थिर std::string&bit,
				     vvp_context_t)
{
      anyedge_string_मूल्य*value = get_string_मूल्य(last_मूल्य[port.port()]);
      निश्चित(value);
      अगर (value->recv_string(bit)) {
	    run_waiting_threads_(threads_);
	    vvp_net_t*net = port.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

/*
 * An anyedge receiving an object should do nothing with it, but should
 * trigger waiting threads.
 */
व्योम vvp_fun_anyedge_sa::recv_object(vvp_net_ptr_t port, vvp_object_t,
				     vvp_context_t)
{
      run_waiting_threads_(threads_);
      vvp_net_t*net = port.ptr();
      net->send_vec4(vvp_vector4_t(), 0);
}

vvp_fun_anyedge_aa::vvp_fun_anyedge_aa()
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(यह, context_scope_);
}

vvp_fun_anyedge_aa::~vvp_fun_anyedge_aa()
{
}

व्योम vvp_fun_anyedge_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, नया vvp_fun_anyedge_state_s);
      reset_instance(context);
}

व्योम vvp_fun_anyedge_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
      क्रम (अचिन्हित idx = 0 ;  idx < 4 ;  idx += 1) {
	    अगर (last_मूल्य[idx])
	          last_मूल्य[idx]->duplicate(state->last_मूल्य[idx]);
	    अन्यथा अगर (state->last_मूल्य[idx])
		  state->last_मूल्य[idx]->reset();
      }
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम vvp_fun_anyedge_aa::free_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_item(context, context_idx_));
      मिटाओ state;
}
#पूर्ण_अगर

vthread_t vvp_fun_anyedge_aa::add_waiting_thread(vthread_t thread)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vthread_get_wt_context_item(context_idx_));

      vthread_t tmp = state->threads;
      state->threads = thread;

      वापस tmp;
}

व्योम vvp_fun_anyedge_aa::recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      अगर (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_vec4_मूल्य*value = get_vec4_मूल्य(state->last_मूल्य[port.port()]);
            निश्चित(value);
            अगर (value->recv_vec4(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(bit, context);
            }
      } अन्यथा {
            context = context_scope_->live_contexts;
            जबतक (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_अग्र_context(context);
            }
            anyedge_vec4_मूल्य*value = get_vec4_मूल्य(last_मूल्य[port.port()]);
            निश्चित(value);
            value->set(bit);
      }
}

व्योम vvp_fun_anyedge_aa::recv_real(vvp_net_ptr_t port, द्विगुन bit,
                                   vvp_context_t context)
{
      अगर (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_real_मूल्य*value = get_real_मूल्य(state->last_मूल्य[port.port()]);
            निश्चित(value);
            अगर (value->recv_real(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } अन्यथा {
            context = context_scope_->live_contexts;
            जबतक (context) {
                  recv_real(port, bit, context);
                  context = vvp_get_अग्र_context(context);
            }
            anyedge_real_मूल्य*value = get_real_मूल्य(last_मूल्य[port.port()]);
            निश्चित(value);
            value->set(bit);
      }
}

व्योम vvp_fun_anyedge_aa::recv_string(vvp_net_ptr_t port, स्थिर std::string&bit,
				     vvp_context_t context)
{
      अगर (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_string_मूल्य*value = get_string_मूल्य(state->last_मूल्य[port.port()]);
            निश्चित(value);
            अगर (value->recv_string(bit)) {
                  run_waiting_threads_(state->threads);
                  vvp_net_t*net = port.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } अन्यथा {
            context = context_scope_->live_contexts;
            जबतक (context) {
                  recv_string(port, bit, context);
                  context = vvp_get_अग्र_context(context);
            }
            anyedge_string_मूल्य*value = get_string_मूल्य(last_मूल्य[port.port()]);
            निश्चित(value);
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

      वापस tmp;
}

व्योम vvp_fun_event_or_sa::recv_vec4(vvp_net_ptr_t, स्थिर vvp_vector4_t&bit,
                                    vvp_context_t)
{
      run_waiting_threads_(threads_);
      base_net_->send_vec4(bit, 0);
}

vvp_fun_event_or_aa::vvp_fun_event_or_aa(vvp_net_t*base_net)
: vvp_fun_event_or(base_net)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(यह, context_scope_);
}

vvp_fun_event_or_aa::~vvp_fun_event_or_aa()
{
}

व्योम vvp_fun_event_or_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, नया waitable_state_s);
}

व्योम vvp_fun_event_or_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम vvp_fun_event_or_aa::free_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_item(context, context_idx_));
      मिटाओ state;
}
#पूर्ण_अगर

vthread_t vvp_fun_event_or_aa::add_waiting_thread(vthread_t thread)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vthread_get_wt_context_item(context_idx_));

      vthread_t tmp = state->threads;
      state->threads = thread;

      वापस tmp;
}

व्योम vvp_fun_event_or_aa::recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                                    vvp_context_t context)
{
      अगर (context) {
            waitable_state_s*state = static_cast<waitable_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            run_waiting_threads_(state->threads);
            base_net_->send_vec4(bit, context);
      } अन्यथा {
            context = context_scope_->live_contexts;
            जबतक (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_अग्र_context(context);
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

      वापस tmp;
}

व्योम vvp_named_event_sa::recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                                   vvp_context_t)
{
      run_waiting_threads_(threads_);
      vvp_net_t*net = port.ptr();
      net->send_vec4(bit, 0);

      __vpiNamedEvent*obj = dynamic_cast<__vpiNamedEvent*>(handle_);
      निश्चित(obj);
      obj->run_vpi_callbacks();
}

vvp_named_event_aa::vvp_named_event_aa(__vpiHandle*h)
: vvp_named_event(h)
{
      context_idx_ = vpip_add_item_to_context(यह, vpip_peek_context_scope());
}

vvp_named_event_aa::~vvp_named_event_aa()
{
}

व्योम vvp_named_event_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, नया waitable_state_s);
}

व्योम vvp_named_event_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->threads = 0;
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम vvp_named_event_aa::free_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_item(context, context_idx_));
      मिटाओ state;
}
#पूर्ण_अगर

vthread_t vvp_named_event_aa::add_waiting_thread(vthread_t thread)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vthread_get_wt_context_item(context_idx_));

      vthread_t tmp = state->threads;
      state->threads = thread;

      वापस tmp;
}

व्योम vvp_named_event_aa::recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      निश्चित(context);

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

अटल व्योम compile_event_or(अक्षर*label, अचिन्हित argc, काष्ठा symb_s*argv);

व्योम compile_event(अक्षर*label, अक्षर*type, अचिन्हित argc, काष्ठा symb_s*argv)
{
      vvp_net_fun_t*fun = 0;

      अगर (type == 0) {
	    compile_event_or(label, argc, argv);
	    वापस;
      }

      अगर (म_भेद(type,"anyedge") == 0) {

	    मुक्त(type);

            अगर (vpip_peek_current_scope()->is_automatic()) {
                  fun = नया vvp_fun_anyedge_aa;
            } अन्यथा {
                  fun = नया vvp_fun_anyedge_sa;
            }

      } अन्यथा {

	    vvp_fun_edge::edge_t edge_type = vvp_edge_none;

	    अगर (म_भेद(type,"posedge") == 0)
		  edge_type = vvp_edge_posedge;
	    अन्यथा अगर (म_भेद(type,"negedge") == 0)
		  edge_type = vvp_edge_negedge;
	    अन्यथा अगर (म_भेद(type,"edge") == 0)
		  edge_type = vvp_edge_edge;

	    निश्चित(argc <= 4);
	    मुक्त(type);

            अगर (vpip_peek_current_scope()->is_automatic()) {
                  fun = नया vvp_fun_edge_aa(edge_type);
            } अन्यथा {
                  fun = नया vvp_fun_edge_sa(edge_type);
            }

      }

      vvp_net_t* ptr = नया vvp_net_t;
      ptr->fun = fun;

      define_functor_प्रतीक(label, ptr);
      मुक्त(label);

      inputs_connect(ptr, argc, argv);
      मुक्त(argv);
}

अटल व्योम compile_event_or(अक्षर*label, अचिन्हित argc, काष्ठा symb_s*argv)
{
      vvp_net_t*base_net = नया vvp_net_t;
      अगर (vpip_peek_current_scope()->is_automatic()) {
            base_net->fun = नया vvp_fun_event_or_aa(base_net);
      } अन्यथा {
            base_net->fun = नया vvp_fun_event_or_sa(base_net);
      }
      define_functor_प्रतीक(label, base_net);
      मुक्त(label);

	/* This is a simplified version of a wide functor. We don't
	   care about the data values or what port they arrived on,
	   so we can use a single shared functor. */
      vvp_net_t*curr_net = base_net;
      क्रम (अचिन्हित idx = 0 ;  idx < argc ;  idx += 1) {
	    अगर (idx > 0 && (idx % 4) == 0) {
		  curr_net = नया vvp_net_t;
		  curr_net->fun = base_net->fun;
	    }
	    निवेश_connect(curr_net, idx % 4, argv[idx].text);
      }
      मुक्त(argv);
}

/*
 * This handles the compile of named events. This functor has no
 * inputs, it is only accessed by behavioral trigger statements, which
 * in vvp are %set instructions.
 */
व्योम compile_named_event(अक्षर*label, अक्षर*name, bool local_flag)
{
      vvp_net_t*ptr = नया vvp_net_t;

      vpiHandle obj = vpip_make_named_event(name, ptr);

      अगर (vpip_peek_current_scope()->is_automatic()) {
            ptr->fun = नया vvp_named_event_aa(obj);
      } अन्यथा {
            ptr->fun = नया vvp_named_event_sa(obj);
      }
      define_functor_प्रतीक(label, ptr);
      compile_vpi_प्रतीक(label, obj);
      अगर (! local_flag) vpip_attach_to_current_scope(obj);

      मुक्त(label);
      मिटाओ[] name;
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम named_event_delete(__vpiHandle*handle)
{
      मिटाओ dynamic_cast<__vpiNamedEvent *>(handle);
}
#पूर्ण_अगर
