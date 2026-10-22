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

using नामाकाश std;

संरचना vvp_fun_part_state_s {
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

शून्य vvp_fun_part_sa::recv_vec4(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                                vvp_context_t)
{
      प्रतिज्ञा(port.port() == 0);

      vvp_vector4_t tmp (bit, base_, wid_);
      यदि (val_ .eeq( tmp ))
	    प्रत्यागम;

      val_ = tmp;

      यदि (net_ == 0) {
	    net_ = port.ptr();
	    schedule_functor(एतत्);
      }
}

/*
 * Handle the case that the part select node is actually fed by a part
 * select assignment. It's not exactly clear what might make this
 * happen, but is does seem to happen and this should have well
 * defined behavior.
 */
शून्य vvp_fun_part_sa::recv_vec4_pv(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t)
{
      vvp_vector4_t tmp (vwid, BIT4_Z);
      tmp.set_vec(base_, val_);
      tmp.set_vec(base, bit);
      recv_vec4(port, tmp, 0);
}

शून्य vvp_fun_part_sa::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;
      ptr->send_vec4(val_, 0);
}

vvp_fun_part_aa::vvp_fun_part_aa(unsigned base, unsigned wid)
: vvp_fun_part(base, wid)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(एतत्, context_scope_);
}

vvp_fun_part_aa::~vvp_fun_part_aa()
{
}

शून्य vvp_fun_part_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, नूतन vvp_vector4_t);
}

शून्य vvp_fun_part_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      val->set_to_x();
}

#ifdef CHECK_WITH_VALGRIND
शून्य vvp_fun_part_aa::free_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));
      delete val;
}
#endif

शून्य vvp_fun_part_aa::recv_vec4(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                                vvp_context_t context)
{
      यदि (context) {
            प्रतिज्ञा(port.port() == 0);

            vvp_vector4_t*val = static_cast<vvp_vector4_t*>
                  (vvp_get_context_item(context, context_idx_));

            vvp_vector4_t tmp (wid_, BIT4_X);
            प्रति (unsigned idx = 0 ;  idx < wid_ ;  idx += 1) {
                  यदि (idx + base_ < bit.size())
                        tmp.set_bit(idx, bit.value(base_+idx));
            }
            यदि (!val->eeq( tmp )) {
                  *val = tmp;
                  port.ptr()->send_vec4(tmp, context);
            }
      } अन्यथा {
            context = context_scope_->live_contexts;
            यावत् (context) {
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
शून्य vvp_fun_part_aa::recv_vec4_pv(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t context)
{
      यदि (context) {
	    ध्रुव vvp_vector4_t*val = static_cast<vvp_vector4_t*>
	                                         (vvp_get_context_item(context,
	                                                               context_idx_));

	    vvp_vector4_t tmp (vwid, BIT4_Z);
	    tmp.set_vec(base_, *val);
	    tmp.set_vec(base, bit);
	    recv_vec4(port, tmp, context);
      } अन्यथा {
	    context = context_scope_->live_contexts;
	    यावत् (context) {
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

शून्य vvp_fun_part_pv::recv_vec4(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                                vvp_context_t context)
{
      प्रतिज्ञा(port.port() == 0);

      यदि (bit.size() != wid_) {
	    cerr << "internal error: part_pv data mismatch. "
		 << "base_=" << base_ << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << bit
		 << endl;
      }
      प्रतिज्ञा(bit.size() == wid_);

      port.ptr()->send_vec4_pv(bit, base_, vwid_, context);
}

शून्य vvp_fun_part_pv::recv_vec4_pv(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                                 unsigned base, unsigned vwid, vvp_context_t ctx)
{
      प्रतिज्ञा(port.port() == 0);
      प्रतिज्ञा(base + bit.size() <= vwid);
      प्रतिज्ञा(vwid == wid_);

      vvp_vector4_t tmp(wid_, BIT4_Z);
      tmp.set_vec(base, bit);

      port.ptr()->send_vec4_pv(tmp, base_, vwid_, ctx);
}

शून्य vvp_fun_part_pv::recv_vec8(vvp_net_ptr_t port, ध्रुव vvp_vector8_t&bit)
{
      प्रतिज्ञा(port.port() == 0);

      यदि (bit.size() != wid_) {
	    cerr << "internal error: part_pv (strength-aware) data mismatch. "
		 << "base_=" << base_ << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << bit
		 << endl;
      }
      प्रतिज्ञा(bit.size() == wid_);

      port.ptr()->send_vec8_pv(bit, base_, vwid_);
}

vvp_fun_part_var::vvp_fun_part_var(unsigned w, तार्किक is_signed)
: wid_(w), is_signed_(is_signed)
{
}

vvp_fun_part_var::~vvp_fun_part_var()
{
}

तार्किक vvp_fun_part_var::recv_vec4_(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                                  पूर्णाङ्क&base, vvp_vector4_t&source,
                                  vvp_vector4_t&ref)
{
      int32_t tmp;
      विकल्प (port.port()) {
	  पक्ष 0:
	    source = bit;
	    भङ्ग;
	  पक्ष 1:
	      // INT_MIN is before the vector and is used to
	      // represent a 'bx value on the select input.
	      // It is also used to guard against 32 bit
	      // unsigned -> signed address overflow / wrap around.
	    यदि (!vector4_to_मूल्य(bit, tmp, is_signed_) || (!is_signed_ && tmp < 0))
		  tmp = INT32_MIN;
	    यदि (static_cast<पूर्णाङ्क>(tmp) == base) प्रत्यागम असत्य;
	    base = tmp;
	    भङ्ग;
	  स्वभाव:
	    fprintf(stderr, "Unsupported port type %u.\n", port.port());
	    प्रतिज्ञा(0);
	    भङ्ग;
      }

      vvp_vector4_t res (wid_);

      प्रति (unsigned idx = 0 ;  idx < wid_ ;  idx += 1) {
	    पूर्णाङ्क adr = base+idx;
	    यदि (adr < 0) अनुवर्त;
	    यदि ((unsigned)adr >= source.size()) भङ्ग;

	    res.set_bit(idx, source.value((unsigned)adr));
      }

      यदि (! ref.eeq(res)) {
	    ref = res;
            प्रत्यागम सत्य;
      }
      प्रत्यागम असत्य;
}

vvp_fun_part_var_sa::vvp_fun_part_var_sa(unsigned w, तार्किक is_signed)
: vvp_fun_part_var(w, is_signed), base_(0)
{
}

vvp_fun_part_var_sa::~vvp_fun_part_var_sa()
{
}

शून्य vvp_fun_part_var_sa::recv_vec4(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                                    vvp_context_t)
{
      यदि (recv_vec4_(port, bit, base_, source_, ref_)) {
	    port.ptr()->send_vec4(ref_, 0);
      }
}

शून्य vvp_fun_part_var_sa::recv_vec4_pv(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
				       unsigned base, unsigned vwid, vvp_context_t)
{
      vvp_vector4_t tmp = source_;
      यदि (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid);

      प्रतिज्ञा(tmp.size() == vwid);
      tmp.set_vec(base, bit);
      recv_vec4(port, tmp, 0);
}

संरचना vvp_fun_part_var_state_s {
      vvp_fun_part_var_state_s() : base(0) { }

      पूर्णाङ्क base;
      vvp_vector4_t source;
      vvp_vector4_t ref;
};

vvp_fun_part_var_aa::vvp_fun_part_var_aa(unsigned w, तार्किक is_signed)
: vvp_fun_part_var(w, is_signed)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(एतत्, context_scope_);
}

vvp_fun_part_var_aa::~vvp_fun_part_var_aa()
{
}

शून्य vvp_fun_part_var_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, नूतन vvp_fun_part_var_state_s);
}

शून्य vvp_fun_part_var_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->base = 0;
      state->source.set_to_x();
      state->ref.set_to_x();
}

#ifdef CHECK_WITH_VALGRIND
शून्य vvp_fun_part_var_aa::free_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_item(context, context_idx_));
      delete state;
}
#endif

शून्य vvp_fun_part_var_aa::recv_vec4(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                                    vvp_context_t context)
{
      यदि (context) {
            vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            यदि (recv_vec4_(port, bit, state->base, state->source, state->ref)) {
                  port.ptr()->send_vec4(state->ref, context);
            }
      } अन्यथा {
            context = context_scope_->live_contexts;
            यावत् (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
      }
}

शून्य vvp_fun_part_var_aa::recv_vec4_pv(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
				       unsigned base, unsigned vwid, vvp_context_t context)
{
      यदि (context) {
	    ध्रुव vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
	                                                      (vvp_get_context_item(context,
	                                                       context_idx_));

            vvp_vector4_t tmp = state->source;
            यदि (tmp.size() == 0)
                  tmp = vvp_vector4_t(vwid);

            प्रतिज्ञा(tmp.size() == vwid);
            tmp.set_vec(base, bit);
            recv_vec4(port, tmp, context);
      } अन्यथा {
            context = context_scope_->live_contexts;
            यावत् (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
      }
}

/*
 * Given a node functor, create a network node and link it into the
 * netlist. This form assumes nodes with a single input.
 */
शून्य link_गाठ_1(वर्ण*label, वर्ण*source, vvp_net_fun_t*fun)
{
      vvp_net_t*net = नूतन vvp_net_t;
      net->fun = fun;

      define_functor_चिन्ह(label, net);
      मुक्त(label);

      आदान_connect(net, 0, source);
}

शून्य compile_part_select(वर्ण*label, वर्ण*source,
			 unsigned base, unsigned wid)
{
      vvp_fun_part*fun = 0;
      यदि (vpip_peek_current_scope()->is_automatic()) {
            fun = नूतन vvp_fun_part_aa(base, wid);
      } अन्यथा {
            fun = नूतन vvp_fun_part_sa(base, wid);
      }
      link_गाठ_1(label, source, fun);
}

शून्य compile_part_select_pv(वर्ण*label, वर्ण*source,
			    unsigned base, unsigned wid,
			    unsigned vector_wid)
{
      vvp_fun_part_pv*fun = नूतन vvp_fun_part_pv(base, wid, vector_wid);
      link_गाठ_1(label, source, fun);
}

शून्य compile_part_select_var(वर्ण*label, वर्ण*source, वर्ण*var,
			     unsigned wid, तार्किक is_signed)
{
      vvp_fun_part_var*fun = 0;
      यदि (vpip_peek_current_scope()->is_automatic()) {
            fun = नूतन vvp_fun_part_var_aa(wid, is_signed);
      } अन्यथा {
            fun = नूतन vvp_fun_part_var_sa(wid, is_signed);
      }
      vvp_net_t*net = नूतन vvp_net_t;
      net->fun = fun;

      define_functor_चिन्ह(label, net);
      मुक्त(label);

      आदान_connect(net, 0, source);
      आदान_connect(net, 1, var);
}
