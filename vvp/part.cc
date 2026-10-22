/*
 * Copyright (c) 2004-2025 Stephen Williams (steve@icarus.com)
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

# define __STDC_LIMIT_MACROS
# include  "compile.h"
# include  "part.h"
# include  <cstdlib>
# include  <climits>
# include  <stdint.h>
# include  <iostream>
# include  <cassert>

using ନାମାକାଶ std;

ସଂରଚନା vvp_fun_part_state_s {
      vvp_fun_part_state_s() {}

      vvp_vector4_t bits;
};

vvp_fun_part::vvp_fun_part(unsigned base, unsigned wid)
: base_(base), wid_(wid)
{
}

vvp_fun_part::~vvp_fun_part()
{
}

vvp_fun_part_sa::vvp_fun_part_sa(unsigned base, unsigned wid)
: vvp_fun_part(base, wid)
{
      net_ = 0;
}

vvp_fun_part_sa::~vvp_fun_part_sa()
{
}

ଶୂନ୍ୟ vvp_fun_part_sa::recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                                vvp_context_t)
{
      ପ୍ରତିଜ୍ଞା(port.port() == 0);

      vvp_vector4_t tmp (bit, base_, wid_);
      ଯଦି (val_ .eeq( tmp ))
	    ଫେରନ୍ତୁ;

      val_ = tmp;

      ଯଦି (net_ == 0) {
	    net_ = port.ptr();
	    schedule_functor(ଏହି);
      }
}

/*
 * Handle the case that the part select node is actually fed by a part
 * select assignment. It's not exactly clear what might make this
 * happen, but is does seem to happen and this should have well
 * defined behavior.
 */
ଶୂନ୍ୟ vvp_fun_part_sa::recv_vec4_pv(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t)
{
      vvp_vector4_t tmp (vwid, BIT4_Z);
      tmp.set_vec(base_, val_);
      tmp.set_vec(base, bit);
      recv_vec4(port, tmp, 0);
}

ଶୂନ୍ୟ vvp_fun_part_sa::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;
      ptr->send_vec4(val_, 0);
}

vvp_fun_part_aa::vvp_fun_part_aa(unsigned base, unsigned wid)
: vvp_fun_part(base, wid)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(ଏହି, context_scope_);
}

vvp_fun_part_aa::~vvp_fun_part_aa()
{
}

ଶୂନ୍ୟ vvp_fun_part_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, ନୂତନ vvp_vector4_t);
}

ଶୂନ୍ୟ vvp_fun_part_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      val->set_to_x();
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ vvp_fun_part_aa::free_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));
      delete val;
}
#endif

ଶୂନ୍ୟ vvp_fun_part_aa::recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                                vvp_context_t context)
{
      ଯଦି (context) {
            ପ୍ରତିଜ୍ଞା(port.port() == 0);

            vvp_vector4_t*val = static_cast<vvp_vector4_t*>
                  (vvp_get_context_item(context, context_idx_));

            vvp_vector4_t tmp (wid_, BIT4_X);
            ପାଇଁ (unsigned idx = 0 ;  idx < wid_ ;  idx += 1) {
                  ଯଦି (idx + base_ < bit.size())
                        tmp.set_bit(idx, bit.value(base_+idx));
            }
            ଯଦି (!val->eeq( tmp )) {
                  *val = tmp;
                  port.ptr()->send_vec4(tmp, context);
            }
      } ଅନ୍ୟଥା {
            context = context_scope_->live_contexts;
            ଯେତେବେଳେ (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
      }
}

/*
 * Handle the case that the part select node is actually fed by a part
 * select assignment. It's not exactly clear what might make this
 * happen, but is does seem to happen and this should have well
 * defined behavior.
 */
ଶୂନ୍ୟ vvp_fun_part_aa::recv_vec4_pv(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t context)
{
      ଯଦି (context) {
	    ଅଚଳ vvp_vector4_t*val = static_cast<vvp_vector4_t*>
	                                         (vvp_get_context_item(context,
	                                                               context_idx_));

	    vvp_vector4_t tmp (vwid, BIT4_Z);
	    tmp.set_vec(base_, *val);
	    tmp.set_vec(base, bit);
	    recv_vec4(port, tmp, context);
      } ଅନ୍ୟଥା {
	    context = context_scope_->live_contexts;
	    ଯେତେବେଳେ (context) {
		  recv_vec4_pv(port, bit, base, vwid, context);
		  context = vvp_get_next_context(context);
	    }
      }
}

vvp_fun_part_pv::vvp_fun_part_pv(unsigned b, unsigned w, unsigned v)
: base_(b), wid_(w), vwid_(v)
{
}

vvp_fun_part_pv::~vvp_fun_part_pv()
{
}

ଶୂନ୍ୟ vvp_fun_part_pv::recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                                vvp_context_t context)
{
      ପ୍ରତିଜ୍ଞା(port.port() == 0);

      ଯଦି (bit.size() != wid_) {
	    cerr << "internal error: part_pv data mismatch. "
		 << "base_=" << base_ << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << bit
		 << endl;
      }
      ପ୍ରତିଜ୍ଞା(bit.size() == wid_);

      port.ptr()->send_vec4_pv(bit, base_, vwid_, context);
}

ଶୂନ୍ୟ vvp_fun_part_pv::recv_vec4_pv(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                                 unsigned base, unsigned vwid, vvp_context_t ctx)
{
      ପ୍ରତିଜ୍ଞା(port.port() == 0);
      ପ୍ରତିଜ୍ଞା(base + bit.size() <= vwid);
      ପ୍ରତିଜ୍ଞା(vwid == wid_);

      vvp_vector4_t tmp(wid_, BIT4_Z);
      tmp.set_vec(base, bit);

      port.ptr()->send_vec4_pv(tmp, base_, vwid_, ctx);
}

ଶୂନ୍ୟ vvp_fun_part_pv::recv_vec8(vvp_net_ptr_t port, ଅଚଳ vvp_vector8_t&bit)
{
      ପ୍ରତିଜ୍ଞା(port.port() == 0);

      ଯଦି (bit.size() != wid_) {
	    cerr << "internal error: part_pv (strength-aware) data mismatch. "
		 << "base_=" << base_ << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << bit
		 << endl;
      }
      ପ୍ରତିଜ୍ଞା(bit.size() == wid_);

      port.ptr()->send_vec8_pv(bit, base_, vwid_);
}

vvp_fun_part_var::vvp_fun_part_var(unsigned w, ତାର୍କିକ is_signed)
: wid_(w), is_signed_(is_signed)
{
}

vvp_fun_part_var::~vvp_fun_part_var()
{
}

ତାର୍କିକ vvp_fun_part_var::recv_vec4_(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                                  ପୂର୍ଣ୍ଣସଂଖ୍ୟା&base, vvp_vector4_t&source,
                                  vvp_vector4_t&ref)
{
      int32_t tmp;
      ବାଛ (port.port()) {
	  ସ୍ଥିତି 0:
	    source = bit;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 1:
	      // INT_MIN is before the vector and is used to
	      // represent a 'bx value on the select input.
	      // It is also used to guard against 32 bit
	      // unsigned -> signed address overflow / wrap around.
	    ଯଦି (!vector4_to_ମୂଲ୍ୟ(bit, tmp, is_signed_) || (!is_signed_ && tmp < 0))
		  tmp = INT32_MIN;
	    ଯଦି (static_cast<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(tmp) == base) ଫେରନ୍ତୁ ମିଥ୍ୟା;
	    base = tmp;
	    ଭାଙ୍ଗ;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "Unsupported port type %u.\n", port.port());
	    ପ୍ରତିଜ୍ଞା(0);
	    ଭାଙ୍ଗ;
      }

      vvp_vector4_t res (wid_);

      ପାଇଁ (unsigned idx = 0 ;  idx < wid_ ;  idx += 1) {
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା adr = base+idx;
	    ଯଦି (adr < 0) ଜାରି;
	    ଯଦି ((unsigned)adr >= source.size()) ଭାଙ୍ଗ;

	    res.set_bit(idx, source.value((unsigned)adr));
      }

      ଯଦି (! ref.eeq(res)) {
	    ref = res;
            ଫେରନ୍ତୁ ସତ୍ୟ;
      }
      ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

vvp_fun_part_var_sa::vvp_fun_part_var_sa(unsigned w, ତାର୍କିକ is_signed)
: vvp_fun_part_var(w, is_signed), base_(0)
{
}

vvp_fun_part_var_sa::~vvp_fun_part_var_sa()
{
}

ଶୂନ୍ୟ vvp_fun_part_var_sa::recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                                    vvp_context_t)
{
      ଯଦି (recv_vec4_(port, bit, base_, source_, ref_)) {
	    port.ptr()->send_vec4(ref_, 0);
      }
}

ଶୂନ୍ୟ vvp_fun_part_var_sa::recv_vec4_pv(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
				       unsigned base, unsigned vwid, vvp_context_t)
{
      vvp_vector4_t tmp = source_;
      ଯଦି (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid);

      ପ୍ରତିଜ୍ଞା(tmp.size() == vwid);
      tmp.set_vec(base, bit);
      recv_vec4(port, tmp, 0);
}

ସଂରଚନା vvp_fun_part_var_state_s {
      vvp_fun_part_var_state_s() : base(0) { }

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା base;
      vvp_vector4_t source;
      vvp_vector4_t ref;
};

vvp_fun_part_var_aa::vvp_fun_part_var_aa(unsigned w, ତାର୍କିକ is_signed)
: vvp_fun_part_var(w, is_signed)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(ଏହି, context_scope_);
}

vvp_fun_part_var_aa::~vvp_fun_part_var_aa()
{
}

ଶୂନ୍ୟ vvp_fun_part_var_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, ନୂତନ vvp_fun_part_var_state_s);
}

ଶୂନ୍ୟ vvp_fun_part_var_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->base = 0;
      state->source.set_to_x();
      state->ref.set_to_x();
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ vvp_fun_part_var_aa::free_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_item(context, context_idx_));
      delete state;
}
#endif

ଶୂନ୍ୟ vvp_fun_part_var_aa::recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                                    vvp_context_t context)
{
      ଯଦି (context) {
            vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            ଯଦି (recv_vec4_(port, bit, state->base, state->source, state->ref)) {
                  port.ptr()->send_vec4(state->ref, context);
            }
      } ଅନ୍ୟଥା {
            context = context_scope_->live_contexts;
            ଯେତେବେଳେ (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
      }
}

ଶୂନ୍ୟ vvp_fun_part_var_aa::recv_vec4_pv(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
				       unsigned base, unsigned vwid, vvp_context_t context)
{
      ଯଦି (context) {
	    ଅଚଳ vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
	                                                      (vvp_get_context_item(context,
	                                                       context_idx_));

            vvp_vector4_t tmp = state->source;
            ଯଦି (tmp.size() == 0)
                  tmp = vvp_vector4_t(vwid);

            ପ୍ରତିଜ୍ଞା(tmp.size() == vwid);
            tmp.set_vec(base, bit);
            recv_vec4(port, tmp, context);
      } ଅନ୍ୟଥା {
            context = context_scope_->live_contexts;
            ଯେତେବେଳେ (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
      }
}

/*
 * Given a node functor, create a network node and link it into the
 * netlist. This form assumes nodes with a single input.
 */
ଶୂନ୍ୟ link_ଗଣ୍ଠି_1(ଅକ୍ଷର*label, ଅକ୍ଷର*source, vvp_net_fun_t*fun)
{
      vvp_net_t*net = ନୂତନ vvp_net_t;
      net->fun = fun;

      define_functor_ପ୍ରତୀକ(label, net);
      ମୁକ୍ତ(label);

      ନିବେଶ_connect(net, 0, source);
}

ଶୂନ୍ୟ compile_part_select(ଅକ୍ଷର*label, ଅକ୍ଷର*source,
			 unsigned base, unsigned wid)
{
      vvp_fun_part*fun = 0;
      ଯଦି (vpip_peek_current_scope()->is_automatic()) {
            fun = ନୂତନ vvp_fun_part_aa(base, wid);
      } ଅନ୍ୟଥା {
            fun = ନୂତନ vvp_fun_part_sa(base, wid);
      }
      link_ଗଣ୍ଠି_1(label, source, fun);
}

ଶୂନ୍ୟ compile_part_select_pv(ଅକ୍ଷର*label, ଅକ୍ଷର*source,
			    unsigned base, unsigned wid,
			    unsigned vector_wid)
{
      vvp_fun_part_pv*fun = ନୂତନ vvp_fun_part_pv(base, wid, vector_wid);
      link_ଗଣ୍ଠି_1(label, source, fun);
}

ଶୂନ୍ୟ compile_part_select_var(ଅକ୍ଷର*label, ଅକ୍ଷର*source, ଅକ୍ଷର*var,
			     unsigned wid, ତାର୍କିକ is_signed)
{
      vvp_fun_part_var*fun = 0;
      ଯଦି (vpip_peek_current_scope()->is_automatic()) {
            fun = ନୂତନ vvp_fun_part_var_aa(wid, is_signed);
      } ଅନ୍ୟଥା {
            fun = ନୂତନ vvp_fun_part_var_sa(wid, is_signed);
      }
      vvp_net_t*net = ନୂତନ vvp_net_t;
      net->fun = fun;

      define_functor_ପ୍ରତୀକ(label, net);
      ମୁକ୍ତ(label);

      ନିବେଶ_connect(net, 0, source);
      ନିବେଶ_connect(net, 1, var);
}
