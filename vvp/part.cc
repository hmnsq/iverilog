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

using namespace std;

काष्ठा vvp_fun_part_state_s {
      vvp_fun_part_state_s() {}

      vvp_vector4_t bits;
};

vvp_fun_part::vvp_fun_part(अचिन्हित base, अचिन्हित wid)
: base_(base), wid_(wid)
{
}

vvp_fun_part::~vvp_fun_part()
{
}

vvp_fun_part_sa::vvp_fun_part_sa(अचिन्हित base, अचिन्हित wid)
: vvp_fun_part(base, wid)
{
      net_ = 0;
}

vvp_fun_part_sa::~vvp_fun_part_sa()
{
}

व्योम vvp_fun_part_sa::recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                                vvp_context_t)
{
      निश्चित(port.port() == 0);

      vvp_vector4_t tmp (bit, base_, wid_);
      अगर (val_ .eeq( tmp ))
	    वापस;

      val_ = tmp;

      अगर (net_ == 0) {
	    net_ = port.ptr();
	    schedule_functor(यह);
      }
}

/*
 * Handle the case that the part select node is actually fed by a part
 * select assignment. It's not exactly clear what might make this
 * happen, but is does seem to happen and this should have well
 * defined behavior.
 */
व्योम vvp_fun_part_sa::recv_vec4_pv(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
				   अचिन्हित base, अचिन्हित vwid, vvp_context_t)
{
      vvp_vector4_t tmp (vwid, BIT4_Z);
      tmp.set_vec(base_, val_);
      tmp.set_vec(base, bit);
      recv_vec4(port, tmp, 0);
}

व्योम vvp_fun_part_sa::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;
      ptr->send_vec4(val_, 0);
}

vvp_fun_part_aa::vvp_fun_part_aa(अचिन्हित base, अचिन्हित wid)
: vvp_fun_part(base, wid)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(यह, context_scope_);
}

vvp_fun_part_aa::~vvp_fun_part_aa()
{
}

व्योम vvp_fun_part_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, नया vvp_vector4_t);
}

व्योम vvp_fun_part_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      val->set_to_x();
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम vvp_fun_part_aa::free_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));
      मिटाओ val;
}
#पूर्ण_अगर

व्योम vvp_fun_part_aa::recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                                vvp_context_t context)
{
      अगर (context) {
            निश्चित(port.port() == 0);

            vvp_vector4_t*val = static_cast<vvp_vector4_t*>
                  (vvp_get_context_item(context, context_idx_));

            vvp_vector4_t tmp (wid_, BIT4_X);
            क्रम (अचिन्हित idx = 0 ;  idx < wid_ ;  idx += 1) {
                  अगर (idx + base_ < bit.size())
                        tmp.set_bit(idx, bit.value(base_+idx));
            }
            अगर (!val->eeq( tmp )) {
                  *val = tmp;
                  port.ptr()->send_vec4(tmp, context);
            }
      } अन्यथा {
            context = context_scope_->live_contexts;
            जबतक (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_अग्र_context(context);
            }
      }
}

/*
 * Handle the case that the part select node is actually fed by a part
 * select assignment. It's not exactly clear what might make this
 * happen, but is does seem to happen and this should have well
 * defined behavior.
 */
व्योम vvp_fun_part_aa::recv_vec4_pv(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
				   अचिन्हित base, अचिन्हित vwid, vvp_context_t context)
{
      अगर (context) {
	    स्थिर vvp_vector4_t*val = static_cast<vvp_vector4_t*>
	                                         (vvp_get_context_item(context,
	                                                               context_idx_));

	    vvp_vector4_t tmp (vwid, BIT4_Z);
	    tmp.set_vec(base_, *val);
	    tmp.set_vec(base, bit);
	    recv_vec4(port, tmp, context);
      } अन्यथा {
	    context = context_scope_->live_contexts;
	    जबतक (context) {
		  recv_vec4_pv(port, bit, base, vwid, context);
		  context = vvp_get_अग्र_context(context);
	    }
      }
}

vvp_fun_part_pv::vvp_fun_part_pv(अचिन्हित b, अचिन्हित w, अचिन्हित v)
: base_(b), wid_(w), vwid_(v)
{
}

vvp_fun_part_pv::~vvp_fun_part_pv()
{
}

व्योम vvp_fun_part_pv::recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                                vvp_context_t context)
{
      निश्चित(port.port() == 0);

      अगर (bit.size() != wid_) {
	    अ_त्रुटि << "internal error: part_pv data mismatch. "
		 << "base_=" << base_ << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << bit
		 << प_पूर्ण;
      }
      निश्चित(bit.size() == wid_);

      port.ptr()->send_vec4_pv(bit, base_, vwid_, context);
}

व्योम vvp_fun_part_pv::recv_vec4_pv(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                                 अचिन्हित base, अचिन्हित vwid, vvp_context_t ctx)
{
      निश्चित(port.port() == 0);
      निश्चित(base + bit.size() <= vwid);
      निश्चित(vwid == wid_);

      vvp_vector4_t tmp(wid_, BIT4_Z);
      tmp.set_vec(base, bit);

      port.ptr()->send_vec4_pv(tmp, base_, vwid_, ctx);
}

व्योम vvp_fun_part_pv::recv_vec8(vvp_net_ptr_t port, स्थिर vvp_vector8_t&bit)
{
      निश्चित(port.port() == 0);

      अगर (bit.size() != wid_) {
	    अ_त्रुटि << "internal error: part_pv (strength-aware) data mismatch. "
		 << "base_=" << base_ << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << bit
		 << प_पूर्ण;
      }
      निश्चित(bit.size() == wid_);

      port.ptr()->send_vec8_pv(bit, base_, vwid_);
}

vvp_fun_part_var::vvp_fun_part_var(अचिन्हित w, bool is_signed)
: wid_(w), is_signed_(is_signed)
{
}

vvp_fun_part_var::~vvp_fun_part_var()
{
}

bool vvp_fun_part_var::recv_vec4_(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                                  पूर्णांक&base, vvp_vector4_t&source,
                                  vvp_vector4_t&ref)
{
      int32_t tmp;
      चयन (port.port()) {
	  हाल 0:
	    source = bit;
	    अवरोध;
	  हाल 1:
	      // INT_MIN is before the vector and is used to
	      // represent a 'bx value on the select input.
	      // It is also used to guard against 32 bit
	      // unsigned -> signed address overflow / wrap around.
	    अगर (!vector4_to_मूल्य(bit, tmp, is_signed_) || (!is_signed_ && tmp < 0))
		  tmp = INT32_MIN;
	    अगर (static_cast<पूर्णांक>(tmp) == base) वापस false;
	    base = tmp;
	    अवरोध;
	  शेष:
	    ख_लिखो(मानक_त्रुटि, "Unsupported port type %u.\n", port.port());
	    निश्चित(0);
	    अवरोध;
      }

      vvp_vector4_t res (wid_);

      क्रम (अचिन्हित idx = 0 ;  idx < wid_ ;  idx += 1) {
	    पूर्णांक adr = base+idx;
	    अगर (adr < 0) जारी;
	    अगर ((अचिन्हित)adr >= source.size()) अवरोध;

	    res.set_bit(idx, source.value((अचिन्हित)adr));
      }

      अगर (! ref.eeq(res)) {
	    ref = res;
            वापस true;
      }
      वापस false;
}

vvp_fun_part_var_sa::vvp_fun_part_var_sa(अचिन्हित w, bool is_signed)
: vvp_fun_part_var(w, is_signed), base_(0)
{
}

vvp_fun_part_var_sa::~vvp_fun_part_var_sa()
{
}

व्योम vvp_fun_part_var_sa::recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                                    vvp_context_t)
{
      अगर (recv_vec4_(port, bit, base_, source_, ref_)) {
	    port.ptr()->send_vec4(ref_, 0);
      }
}

व्योम vvp_fun_part_var_sa::recv_vec4_pv(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
				       अचिन्हित base, अचिन्हित vwid, vvp_context_t)
{
      vvp_vector4_t tmp = source_;
      अगर (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid);

      निश्चित(tmp.size() == vwid);
      tmp.set_vec(base, bit);
      recv_vec4(port, tmp, 0);
}

काष्ठा vvp_fun_part_var_state_s {
      vvp_fun_part_var_state_s() : base(0) { }

      पूर्णांक base;
      vvp_vector4_t source;
      vvp_vector4_t ref;
};

vvp_fun_part_var_aa::vvp_fun_part_var_aa(अचिन्हित w, bool is_signed)
: vvp_fun_part_var(w, is_signed)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_add_item_to_context(यह, context_scope_);
}

vvp_fun_part_var_aa::~vvp_fun_part_var_aa()
{
}

व्योम vvp_fun_part_var_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, नया vvp_fun_part_var_state_s);
}

व्योम vvp_fun_part_var_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->base = 0;
      state->source.set_to_x();
      state->ref.set_to_x();
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम vvp_fun_part_var_aa::free_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_item(context, context_idx_));
      मिटाओ state;
}
#पूर्ण_अगर

व्योम vvp_fun_part_var_aa::recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                                    vvp_context_t context)
{
      अगर (context) {
            vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            अगर (recv_vec4_(port, bit, state->base, state->source, state->ref)) {
                  port.ptr()->send_vec4(state->ref, context);
            }
      } अन्यथा {
            context = context_scope_->live_contexts;
            जबतक (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_अग्र_context(context);
            }
      }
}

व्योम vvp_fun_part_var_aa::recv_vec4_pv(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
				       अचिन्हित base, अचिन्हित vwid, vvp_context_t context)
{
      अगर (context) {
	    स्थिर vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
	                                                      (vvp_get_context_item(context,
	                                                       context_idx_));

            vvp_vector4_t tmp = state->source;
            अगर (tmp.size() == 0)
                  tmp = vvp_vector4_t(vwid);

            निश्चित(tmp.size() == vwid);
            tmp.set_vec(base, bit);
            recv_vec4(port, tmp, context);
      } अन्यथा {
            context = context_scope_->live_contexts;
            जबतक (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_अग्र_context(context);
            }
      }
}

/*
 * Given a node functor, create a network node and link it into the
 * netlist. This form assumes nodes with a single input.
 */
व्योम link_गाँठ_1(अक्षर*label, अक्षर*source, vvp_net_fun_t*fun)
{
      vvp_net_t*net = नया vvp_net_t;
      net->fun = fun;

      define_functor_प्रतीक(label, net);
      मुक्त(label);

      निवेश_connect(net, 0, source);
}

व्योम compile_part_select(अक्षर*label, अक्षर*source,
			 अचिन्हित base, अचिन्हित wid)
{
      vvp_fun_part*fun = 0;
      अगर (vpip_peek_current_scope()->is_automatic()) {
            fun = नया vvp_fun_part_aa(base, wid);
      } अन्यथा {
            fun = नया vvp_fun_part_sa(base, wid);
      }
      link_गाँठ_1(label, source, fun);
}

व्योम compile_part_select_pv(अक्षर*label, अक्षर*source,
			    अचिन्हित base, अचिन्हित wid,
			    अचिन्हित vector_wid)
{
      vvp_fun_part_pv*fun = नया vvp_fun_part_pv(base, wid, vector_wid);
      link_गाँठ_1(label, source, fun);
}

व्योम compile_part_select_var(अक्षर*label, अक्षर*source, अक्षर*var,
			     अचिन्हित wid, bool is_signed)
{
      vvp_fun_part_var*fun = 0;
      अगर (vpip_peek_current_scope()->is_automatic()) {
            fun = नया vvp_fun_part_var_aa(wid, is_signed);
      } अन्यथा {
            fun = नया vvp_fun_part_var_sa(wid, is_signed);
      }
      vvp_net_t*net = नया vvp_net_t;
      net->fun = fun;

      define_functor_प्रतीक(label, net);
      मुक्त(label);

      निवेश_connect(net, 0, source);
      निवेश_connect(net, 1, var);
}
