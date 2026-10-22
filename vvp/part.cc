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

using നാമാകാശ std;

ഘടന vvp_fun_part_state_s {
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

ശൂന്യം vvp_fun_part_sa::recv_vec4(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
                                vvp_context_t)
{
      പ്രതിജ്ഞാ(port.port() == 0);

      vvp_vector4_t tmp (bit, base_, wid_);
      എങ്കിൽ (val_ .eeq( tmp ))
	    തിരികെ;

      val_ = tmp;

      എങ്കിൽ (net_ == 0) {
	    net_ = port.ptr();
	    schedule_functor(ഇത്);
      }
}

/*
 * Handle the case that the part select node is actually fed by a part
 * select assignment. It's not exactly clear what might make this
 * happen, but is does seem to happen and this should have well
 * defined behavior.
 */
ശൂന്യം vvp_fun_part_sa::recv_vec4_pv(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t)
{
      vvp_vector4_t tmp (vwid, BIT4_Z);
      tmp.set_vec(base_, val_);
      tmp.set_vec(base, bit);
      recv_vec4(port, tmp, 0);
}

ശൂന്യം vvp_fun_part_sa::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;
      ptr->send_vec4(val_, 0);
}

vvp_fun_part_aa::vvp_fun_part_aa(unsigned base, unsigned wid)
: vvp_fun_part(base, wid)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(ഇത്, context_scope_);
}

vvp_fun_part_aa::~vvp_fun_part_aa()
{
}

ശൂന്യം vvp_fun_part_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, പുതിയ vvp_vector4_t);
}

ശൂന്യം vvp_fun_part_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      val->set_to_x();
}

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം vvp_fun_part_aa::free_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));
      delete val;
}
#endif

ശൂന്യം vvp_fun_part_aa::recv_vec4(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
                                vvp_context_t context)
{
      എങ്കിൽ (context) {
            പ്രതിജ്ഞാ(port.port() == 0);

            vvp_vector4_t*val = static_cast<vvp_vector4_t*>
                  (vvp_get_context_item(context, context_idx_));

            vvp_vector4_t tmp (wid_, BIT4_X);
            വേണ്ടി (unsigned idx = 0 ;  idx < wid_ ;  idx += 1) {
                  എങ്കിൽ (idx + base_ < bit.size())
                        tmp.set_bit(idx, bit.value(base_+idx));
            }
            എങ്കിൽ (!val->eeq( tmp )) {
                  *val = tmp;
                  port.ptr()->send_vec4(tmp, context);
            }
      } അല്ലെങ്കിൽ {
            context = context_scope_->live_contexts;
            ആകുമ്പോൾ (context) {
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
ശൂന്യം vvp_fun_part_aa::recv_vec4_pv(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t context)
{
      എങ്കിൽ (context) {
	    സ്ഥിരം vvp_vector4_t*val = static_cast<vvp_vector4_t*>
	                                         (vvp_get_context_item(context,
	                                                               context_idx_));

	    vvp_vector4_t tmp (vwid, BIT4_Z);
	    tmp.set_vec(base_, *val);
	    tmp.set_vec(base, bit);
	    recv_vec4(port, tmp, context);
      } അല്ലെങ്കിൽ {
	    context = context_scope_->live_contexts;
	    ആകുമ്പോൾ (context) {
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

ശൂന്യം vvp_fun_part_pv::recv_vec4(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
                                vvp_context_t context)
{
      പ്രതിജ്ഞാ(port.port() == 0);

      എങ്കിൽ (bit.size() != wid_) {
	    cerr << "internal error: part_pv data mismatch. "
		 << "base_=" << base_ << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << bit
		 << endl;
      }
      പ്രതിജ്ഞാ(bit.size() == wid_);

      port.ptr()->send_vec4_pv(bit, base_, vwid_, context);
}

ശൂന്യം vvp_fun_part_pv::recv_vec4_pv(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
                                 unsigned base, unsigned vwid, vvp_context_t ctx)
{
      പ്രതിജ്ഞാ(port.port() == 0);
      പ്രതിജ്ഞാ(base + bit.size() <= vwid);
      പ്രതിജ്ഞാ(vwid == wid_);

      vvp_vector4_t tmp(wid_, BIT4_Z);
      tmp.set_vec(base, bit);

      port.ptr()->send_vec4_pv(tmp, base_, vwid_, ctx);
}

ശൂന്യം vvp_fun_part_pv::recv_vec8(vvp_net_ptr_t port, സ്ഥിരം vvp_vector8_t&bit)
{
      പ്രതിജ്ഞാ(port.port() == 0);

      എങ്കിൽ (bit.size() != wid_) {
	    cerr << "internal error: part_pv (strength-aware) data mismatch. "
		 << "base_=" << base_ << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << bit
		 << endl;
      }
      പ്രതിജ്ഞാ(bit.size() == wid_);

      port.ptr()->send_vec8_pv(bit, base_, vwid_);
}

vvp_fun_part_var::vvp_fun_part_var(unsigned w, തർക്കം is_signed)
: wid_(w), is_signed_(is_signed)
{
}

vvp_fun_part_var::~vvp_fun_part_var()
{
}

തർക്കം vvp_fun_part_var::recv_vec4_(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
                                  പൂർണ്ണസംഖ്യ&base, vvp_vector4_t&source,
                                  vvp_vector4_t&ref)
{
      int32_t tmp;
      തിരഞ്ഞെടുപ്പ് (port.port()) {
	  അവസ്ഥ 0:
	    source = bit;
	    നിർത്തുക;
	  അവസ്ഥ 1:
	      // INT_MIN is before the vector and is used to
	      // represent a 'bx value on the select input.
	      // It is also used to guard against 32 bit
	      // unsigned -> signed address overflow / wrap around.
	    എങ്കിൽ (!vector4_to_മൂല്യം(bit, tmp, is_signed_) || (!is_signed_ && tmp < 0))
		  tmp = INT32_MIN;
	    എങ്കിൽ (static_cast<പൂർണ്ണസംഖ്യ>(tmp) == base) തിരികെ അസത്യം;
	    base = tmp;
	    നിർത്തുക;
	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "Unsupported port type %u.\n", port.port());
	    പ്രതിജ്ഞാ(0);
	    നിർത്തുക;
      }

      vvp_vector4_t res (wid_);

      വേണ്ടി (unsigned idx = 0 ;  idx < wid_ ;  idx += 1) {
	    പൂർണ്ണസംഖ്യ adr = base+idx;
	    എങ്കിൽ (adr < 0) തുടരുക;
	    എങ്കിൽ ((unsigned)adr >= source.size()) നിർത്തുക;

	    res.set_bit(idx, source.value((unsigned)adr));
      }

      എങ്കിൽ (! ref.eeq(res)) {
	    ref = res;
            തിരികെ സത്യം;
      }
      തിരികെ അസത്യം;
}

vvp_fun_part_var_sa::vvp_fun_part_var_sa(unsigned w, തർക്കം is_signed)
: vvp_fun_part_var(w, is_signed), base_(0)
{
}

vvp_fun_part_var_sa::~vvp_fun_part_var_sa()
{
}

ശൂന്യം vvp_fun_part_var_sa::recv_vec4(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
                                    vvp_context_t)
{
      എങ്കിൽ (recv_vec4_(port, bit, base_, source_, ref_)) {
	    port.ptr()->send_vec4(ref_, 0);
      }
}

ശൂന്യം vvp_fun_part_var_sa::recv_vec4_pv(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
				       unsigned base, unsigned vwid, vvp_context_t)
{
      vvp_vector4_t tmp = source_;
      എങ്കിൽ (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid);

      പ്രതിജ്ഞാ(tmp.size() == vwid);
      tmp.set_vec(base, bit);
      recv_vec4(port, tmp, 0);
}

ഘടന vvp_fun_part_var_state_s {
      vvp_fun_part_var_state_s() : base(0) { }

      പൂർണ്ണസംഖ്യ base;
      vvp_vector4_t source;
      vvp_vector4_t ref;
};

vvp_fun_part_var_aa::vvp_fun_part_var_aa(unsigned w, തർക്കം is_signed)
: vvp_fun_part_var(w, is_signed)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(ഇത്, context_scope_);
}

vvp_fun_part_var_aa::~vvp_fun_part_var_aa()
{
}

ശൂന്യം vvp_fun_part_var_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, പുതിയ vvp_fun_part_var_state_s);
}

ശൂന്യം vvp_fun_part_var_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->base = 0;
      state->source.set_to_x();
      state->ref.set_to_x();
}

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം vvp_fun_part_var_aa::free_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_item(context, context_idx_));
      delete state;
}
#endif

ശൂന്യം vvp_fun_part_var_aa::recv_vec4(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
                                    vvp_context_t context)
{
      എങ്കിൽ (context) {
            vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            എങ്കിൽ (recv_vec4_(port, bit, state->base, state->source, state->ref)) {
                  port.ptr()->send_vec4(state->ref, context);
            }
      } അല്ലെങ്കിൽ {
            context = context_scope_->live_contexts;
            ആകുമ്പോൾ (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
      }
}

ശൂന്യം vvp_fun_part_var_aa::recv_vec4_pv(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
				       unsigned base, unsigned vwid, vvp_context_t context)
{
      എങ്കിൽ (context) {
	    സ്ഥിരം vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
	                                                      (vvp_get_context_item(context,
	                                                       context_idx_));

            vvp_vector4_t tmp = state->source;
            എങ്കിൽ (tmp.size() == 0)
                  tmp = vvp_vector4_t(vwid);

            പ്രതിജ്ഞാ(tmp.size() == vwid);
            tmp.set_vec(base, bit);
            recv_vec4(port, tmp, context);
      } അല്ലെങ്കിൽ {
            context = context_scope_->live_contexts;
            ആകുമ്പോൾ (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
      }
}

/*
 * Given a node functor, create a network node and link it into the
 * netlist. This form assumes nodes with a single input.
 */
ശൂന്യം link_കണ്ണി_1(അക്ഷരം*label, അക്ഷരം*source, vvp_net_fun_t*fun)
{
      vvp_net_t*net = പുതിയ vvp_net_t;
      net->fun = fun;

      define_functor_ചിഹ്നം(label, net);
      സ്വതന്ത്രം(label);

      നിവേശം_connect(net, 0, source);
}

ശൂന്യം compile_part_select(അക്ഷരം*label, അക്ഷരം*source,
			 unsigned base, unsigned wid)
{
      vvp_fun_part*fun = 0;
      എങ്കിൽ (vpip_peek_current_scope()->is_automatic()) {
            fun = പുതിയ vvp_fun_part_aa(base, wid);
      } അല്ലെങ്കിൽ {
            fun = പുതിയ vvp_fun_part_sa(base, wid);
      }
      link_കണ്ണി_1(label, source, fun);
}

ശൂന്യം compile_part_select_pv(അക്ഷരം*label, അക്ഷരം*source,
			    unsigned base, unsigned wid,
			    unsigned vector_wid)
{
      vvp_fun_part_pv*fun = പുതിയ vvp_fun_part_pv(base, wid, vector_wid);
      link_കണ്ണി_1(label, source, fun);
}

ശൂന്യം compile_part_select_var(അക്ഷരം*label, അക്ഷരം*source, അക്ഷരം*var,
			     unsigned wid, തർക്കം is_signed)
{
      vvp_fun_part_var*fun = 0;
      എങ്കിൽ (vpip_peek_current_scope()->is_automatic()) {
            fun = പുതിയ vvp_fun_part_var_aa(wid, is_signed);
      } അല്ലെങ്കിൽ {
            fun = പുതിയ vvp_fun_part_var_sa(wid, is_signed);
      }
      vvp_net_t*net = പുതിയ vvp_net_t;
      net->fun = fun;

      define_functor_ചിഹ്നം(label, net);
      സ്വതന്ത്രം(label);

      നിവേശം_connect(net, 0, source);
      നിവേശം_connect(net, 1, var);
}
