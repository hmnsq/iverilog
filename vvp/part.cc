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

using நாமாகாஶ std;

கட்டமைப்பு vvp_fun_part_state_s {
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

வெறுமை vvp_fun_part_sa::recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                                vvp_context_t)
{
      ப்ரதிஜ்ஞா(port.port() == 0);

      vvp_vector4_t tmp (bit, base_, wid_);
      எனில் (val_ .eeq( tmp ))
	    திரும்பு;

      val_ = tmp;

      எனில் (net_ == 0) {
	    net_ = port.ptr();
	    schedule_functor(இது);
      }
}

/*
 * Handle the case that the part select node is actually fed by a part
 * select assignment. It's not exactly clear what might make this
 * happen, but is does seem to happen and this should have well
 * defined behavior.
 */
வெறுமை vvp_fun_part_sa::recv_vec4_pv(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t)
{
      vvp_vector4_t tmp (vwid, BIT4_Z);
      tmp.set_vec(base_, val_);
      tmp.set_vec(base, bit);
      recv_vec4(port, tmp, 0);
}

வெறுமை vvp_fun_part_sa::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;
      ptr->send_vec4(val_, 0);
}

vvp_fun_part_aa::vvp_fun_part_aa(unsigned base, unsigned wid)
: vvp_fun_part(base, wid)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(இது, context_scope_);
}

vvp_fun_part_aa::~vvp_fun_part_aa()
{
}

வெறுமை vvp_fun_part_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, புதிய vvp_vector4_t);
}

வெறுமை vvp_fun_part_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      val->set_to_x();
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை vvp_fun_part_aa::free_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));
      delete val;
}
#endif

வெறுமை vvp_fun_part_aa::recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                                vvp_context_t context)
{
      எனில் (context) {
            ப்ரதிஜ்ஞா(port.port() == 0);

            vvp_vector4_t*val = static_cast<vvp_vector4_t*>
                  (vvp_get_context_item(context, context_idx_));

            vvp_vector4_t tmp (wid_, BIT4_X);
            க்கு (unsigned idx = 0 ;  idx < wid_ ;  idx += 1) {
                  எனில் (idx + base_ < bit.size())
                        tmp.set_bit(idx, bit.value(base_+idx));
            }
            எனில் (!val->eeq( tmp )) {
                  *val = tmp;
                  port.ptr()->send_vec4(tmp, context);
            }
      } இல்லையெனில் {
            context = context_scope_->live_contexts;
            போது (context) {
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
வெறுமை vvp_fun_part_aa::recv_vec4_pv(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t context)
{
      எனில் (context) {
	    மாறிலி vvp_vector4_t*val = static_cast<vvp_vector4_t*>
	                                         (vvp_get_context_item(context,
	                                                               context_idx_));

	    vvp_vector4_t tmp (vwid, BIT4_Z);
	    tmp.set_vec(base_, *val);
	    tmp.set_vec(base, bit);
	    recv_vec4(port, tmp, context);
      } இல்லையெனில் {
	    context = context_scope_->live_contexts;
	    போது (context) {
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

வெறுமை vvp_fun_part_pv::recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                                vvp_context_t context)
{
      ப்ரதிஜ்ஞா(port.port() == 0);

      எனில் (bit.size() != wid_) {
	    cerr << "internal error: part_pv data mismatch. "
		 << "base_=" << base_ << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << bit
		 << endl;
      }
      ப்ரதிஜ்ஞா(bit.size() == wid_);

      port.ptr()->send_vec4_pv(bit, base_, vwid_, context);
}

வெறுமை vvp_fun_part_pv::recv_vec4_pv(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                                 unsigned base, unsigned vwid, vvp_context_t ctx)
{
      ப்ரதிஜ்ஞா(port.port() == 0);
      ப்ரதிஜ்ஞா(base + bit.size() <= vwid);
      ப்ரதிஜ்ஞா(vwid == wid_);

      vvp_vector4_t tmp(wid_, BIT4_Z);
      tmp.set_vec(base, bit);

      port.ptr()->send_vec4_pv(tmp, base_, vwid_, ctx);
}

வெறுமை vvp_fun_part_pv::recv_vec8(vvp_net_ptr_t port, மாறிலி vvp_vector8_t&bit)
{
      ப்ரதிஜ்ஞா(port.port() == 0);

      எனில் (bit.size() != wid_) {
	    cerr << "internal error: part_pv (strength-aware) data mismatch. "
		 << "base_=" << base_ << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << bit
		 << endl;
      }
      ப்ரதிஜ்ஞா(bit.size() == wid_);

      port.ptr()->send_vec8_pv(bit, base_, vwid_);
}

vvp_fun_part_var::vvp_fun_part_var(unsigned w, தருக்க is_signed)
: wid_(w), is_signed_(is_signed)
{
}

vvp_fun_part_var::~vvp_fun_part_var()
{
}

தருக்க vvp_fun_part_var::recv_vec4_(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                                  முழு&base, vvp_vector4_t&source,
                                  vvp_vector4_t&ref)
{
      int32_t tmp;
      தேர்வு (port.port()) {
	  நிலை 0:
	    source = bit;
	    நிறுத்து;
	  நிலை 1:
	      // INT_MIN is before the vector and is used to
	      // represent a 'bx value on the select input.
	      // It is also used to guard against 32 bit
	      // unsigned -> signed address overflow / wrap around.
	    எனில் (!vector4_to_மதிப்பு(bit, tmp, is_signed_) || (!is_signed_ && tmp < 0))
		  tmp = INT32_MIN;
	    எனில் (static_cast<முழு>(tmp) == base) திரும்பு பொய்;
	    base = tmp;
	    நிறுத்து;
	  இயல்பு:
	    fprintf(stderr, "Unsupported port type %u.\n", port.port());
	    ப்ரதிஜ்ஞா(0);
	    நிறுத்து;
      }

      vvp_vector4_t res (wid_);

      க்கு (unsigned idx = 0 ;  idx < wid_ ;  idx += 1) {
	    முழு adr = base+idx;
	    எனில் (adr < 0) தொடர்;
	    எனில் ((unsigned)adr >= source.size()) நிறுத்து;

	    res.set_bit(idx, source.value((unsigned)adr));
      }

      எனில் (! ref.eeq(res)) {
	    ref = res;
            திரும்பு உண்மை;
      }
      திரும்பு பொய்;
}

vvp_fun_part_var_sa::vvp_fun_part_var_sa(unsigned w, தருக்க is_signed)
: vvp_fun_part_var(w, is_signed), base_(0)
{
}

vvp_fun_part_var_sa::~vvp_fun_part_var_sa()
{
}

வெறுமை vvp_fun_part_var_sa::recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                                    vvp_context_t)
{
      எனில் (recv_vec4_(port, bit, base_, source_, ref_)) {
	    port.ptr()->send_vec4(ref_, 0);
      }
}

வெறுமை vvp_fun_part_var_sa::recv_vec4_pv(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
				       unsigned base, unsigned vwid, vvp_context_t)
{
      vvp_vector4_t tmp = source_;
      எனில் (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid);

      ப்ரதிஜ்ஞா(tmp.size() == vwid);
      tmp.set_vec(base, bit);
      recv_vec4(port, tmp, 0);
}

கட்டமைப்பு vvp_fun_part_var_state_s {
      vvp_fun_part_var_state_s() : base(0) { }

      முழு base;
      vvp_vector4_t source;
      vvp_vector4_t ref;
};

vvp_fun_part_var_aa::vvp_fun_part_var_aa(unsigned w, தருக்க is_signed)
: vvp_fun_part_var(w, is_signed)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(இது, context_scope_);
}

vvp_fun_part_var_aa::~vvp_fun_part_var_aa()
{
}

வெறுமை vvp_fun_part_var_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, புதிய vvp_fun_part_var_state_s);
}

வெறுமை vvp_fun_part_var_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->base = 0;
      state->source.set_to_x();
      state->ref.set_to_x();
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை vvp_fun_part_var_aa::free_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_item(context, context_idx_));
      delete state;
}
#endif

வெறுமை vvp_fun_part_var_aa::recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                                    vvp_context_t context)
{
      எனில் (context) {
            vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            எனில் (recv_vec4_(port, bit, state->base, state->source, state->ref)) {
                  port.ptr()->send_vec4(state->ref, context);
            }
      } இல்லையெனில் {
            context = context_scope_->live_contexts;
            போது (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
      }
}

வெறுமை vvp_fun_part_var_aa::recv_vec4_pv(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
				       unsigned base, unsigned vwid, vvp_context_t context)
{
      எனில் (context) {
	    மாறிலி vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
	                                                      (vvp_get_context_item(context,
	                                                       context_idx_));

            vvp_vector4_t tmp = state->source;
            எனில் (tmp.size() == 0)
                  tmp = vvp_vector4_t(vwid);

            ப்ரதிஜ்ஞா(tmp.size() == vwid);
            tmp.set_vec(base, bit);
            recv_vec4(port, tmp, context);
      } இல்லையெனில் {
            context = context_scope_->live_contexts;
            போது (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
      }
}

/*
 * Given a node functor, create a network node and link it into the
 * netlist. This form assumes nodes with a single input.
 */
வெறுமை link_முனை_1(எழுத்து*label, எழுத்து*source, vvp_net_fun_t*fun)
{
      vvp_net_t*net = புதிய vvp_net_t;
      net->fun = fun;

      define_functor_குறியீடு(label, net);
      விடுவி(label);

      உள்ளீடு_connect(net, 0, source);
}

வெறுமை compile_part_select(எழுத்து*label, எழுத்து*source,
			 unsigned base, unsigned wid)
{
      vvp_fun_part*fun = 0;
      எனில் (vpip_peek_current_scope()->is_automatic()) {
            fun = புதிய vvp_fun_part_aa(base, wid);
      } இல்லையெனில் {
            fun = புதிய vvp_fun_part_sa(base, wid);
      }
      link_முனை_1(label, source, fun);
}

வெறுமை compile_part_select_pv(எழுத்து*label, எழுத்து*source,
			    unsigned base, unsigned wid,
			    unsigned vector_wid)
{
      vvp_fun_part_pv*fun = புதிய vvp_fun_part_pv(base, wid, vector_wid);
      link_முனை_1(label, source, fun);
}

வெறுமை compile_part_select_var(எழுத்து*label, எழுத்து*source, எழுத்து*var,
			     unsigned wid, தருக்க is_signed)
{
      vvp_fun_part_var*fun = 0;
      எனில் (vpip_peek_current_scope()->is_automatic()) {
            fun = புதிய vvp_fun_part_var_aa(wid, is_signed);
      } இல்லையெனில் {
            fun = புதிய vvp_fun_part_var_sa(wid, is_signed);
      }
      vvp_net_t*net = புதிய vvp_net_t;
      net->fun = fun;

      define_functor_குறியீடு(label, net);
      விடுவி(label);

      உள்ளீடு_connect(net, 0, source);
      உள்ளீடு_connect(net, 1, var);
}
