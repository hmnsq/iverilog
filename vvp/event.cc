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

शून्य waitable_hooks_s::सततम्_प्रतीक्षा_तन्तवः(vthread_t&तन्तवः)
{
	// Run the non-blocking event controls.
      चरमः = &event_ctls;
      प्रति (evctl*cur = event_ctls; cur != 0;) {
	    यदि (cur->dec_and_सततम्()) {
		  evctl*nxt = cur->अग्र_2;
		  विलोपय cur;
		  cur = nxt;
		  *चरमः = cur;
	    } अन्यथा {
		  चरमः = &(cur->अग्र_2);
		  cur = cur->अग्र_2;
	    }
      }

      vthread_t tmp = तन्तवः;
      यदि (tmp == 0) प्रत्यागम;
      तन्तवः = 0;

      vthread_कालसारणी_list(tmp);
}

evctl::evctl(uint64_t ecount)
{
      ecount_ = ecount;
      अग्र_2 = 0;
}

तार्किक evctl::dec_and_सततम्()
{
      प्रतिज्ञा(ecount_ != 0);

      ecount_ -= 1;
      यदि (ecount_ == 0) सततम्_सततम्();

      प्रत्यागम ecount_ == 0;
}

evctl_real::evctl_real(__vpiHandle*handle, द्विगुण value,
                       uint64_t ecount)
:evctl(ecount)
{
      handle_ = handle;
      मानम्_2 = value;
}

शून्य evctl_real::सततम्_सततम्()
{
      t_vpi_मानम् val;

      val.format = vpiRealVal;
      val.value.real = मानम्_2;
      vpi_put_मानम्(handle_, &val, 0, vpiNoDelay);
}

शून्य कालसारणी_evctl(__vpiHandle*handle, द्विगुण value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      प्रतिज्ञा(ep);
	// Now add this call to the end of the event list.
      *(ep->चरमः) = नूतन evctl_real(handle, value, ecount);
      ep->चरमः = &((*(ep->चरमः))->अग्र_2);
}

evctl_सदिशम्::evctl_सदिशम्(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&value,
                           अचिह्नित off, अचिह्नित wid, uint64_t ecount)
:evctl(ecount), ptr_(ptr), मानम्_2(value)
{
      off_ = off;
      wid_ = wid;
}

शून्य evctl_सदिशम्::सततम्_सततम्()
{
      यदि (wid_ != 0) {
	    vvp_send_vec4_pv(ptr_, मानम्_2, off_, wid_, 0);
      } अन्यथा {
	    vvp_send_vec4(ptr_, मानम्_2, 0);
      }
}

शून्य कालसारणी_evctl(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&value,
                    अचिह्नित विस्थापनम्_2, अचिह्नित wid,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      प्रतिज्ञा(ep);
	// Now add this call to the end of the event list.
      *(ep->चरमः) = नूतन evctl_सदिशम्(ptr, value, विस्थापनम्_2, wid, ecount);
      ep->चरमः = &((*(ep->चरमः))->अग्र_2);
}

evctl_array::evctl_array(vvp_array_t स्मृतिः, अचिह्नित सूचकांक,
                         ध्रुव vvp_vector4_t&value, अचिह्नित off,
                         uint64_t ecount)
:evctl(ecount), मानम्_2(value)
{
      mem_ = स्मृतिः;
      idx_ = सूचकांक;
      off_ = off;
}

शून्य evctl_array::सततम्_सततम्()
{
      mem_->समुच्चयः_word(idx_, off_, मानम्_2);
}

शून्य कालसारणी_evctl(vvp_array_t स्मृतिः, अचिह्नित सूचकांक,
                    ध्रुव vvp_vector4_t&value, अचिह्नित विस्थापनम्_2,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      प्रतिज्ञा(ep);
	// Now add this call to the end of the event list.
      *(ep->चरमः) = नूतन evctl_array(स्मृतिः, सूचकांक, value, विस्थापनम्_2, ecount);
      ep->चरमः = &((*(ep->चरमः))->अग्र_2);
}

evctl_array_r::evctl_array_r(vvp_array_t स्मृतिः, अचिह्नित सूचकांक,
                             द्विगुण value, uint64_t ecount)
:evctl(ecount)
{
      mem_ = स्मृतिः;
      idx_ = सूचकांक;
      मानम्_2 = value;
}

शून्य evctl_array_r::सततम्_सततम्()
{
      mem_->समुच्चयः_word(idx_, मानम्_2);
}

शून्य कालसारणी_evctl(vvp_array_t स्मृतिः, अचिह्नित सूचकांक,
                    द्विगुण value,
                    vvp_net_t*event, uint64_t ecount)
{
	// Get the functor we are going to wait on.
      waitable_hooks_s*ep = dynamic_cast<waitable_hooks_s*> (event->fun);
      प्रतिज्ञा(ep);
	// Now add this call to the end of the event list.
      *(ep->चरमः) = नूतन evctl_array_r(स्मृतिः, सूचकांक, value, ecount);
      ep->चरमः = &((*(ep->चरमः))->अग्र_2);
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
            प्रति (अचिह्नित idx = 0 ;  idx < 4 ;  idx += 1)
                  द्वयङ्काः[idx] = BIT4_X;
      }

      vvp_bit4_t द्वयङ्काः[4];
};

vvp_fun_edge::vvp_fun_edge(edge_t e)
: edge_(e)
{
      प्रति (अचिह्नित idx = 0 ;  idx < 4 ;  idx += 1)
            द्वयङ्काः_2[idx] = BIT4_X;
}

vvp_fun_edge::~vvp_fun_edge()
{
}

तार्किक vvp_fun_edge::recv_vec4_(ध्रुव vvp_vector4_t&bit,
                              vvp_bit4_t&old_bit, vthread_t&तन्तवः)
{
	/* See what kind of edge this represents. */
      edge_t आवरणम् = VVP_EDGE(old_bit, bit.value(0));

	/* Save the current input for the next time around. */
      old_bit = bit.value(0);

      यदि ((edge_ == vvp_edge_none) || (edge_ & आवरणम्)) {
	    सततम्_प्रतीक्षा_तन्तवः(तन्तवः);
            प्रत्यागम सत्य;
      }
      प्रत्यागम असत्य;
}

vvp_fun_edge_sa::vvp_fun_edge_sa(edge_t e)
: vvp_fun_edge(e), तन्तवः_2(0)
{
}

vvp_fun_edge_sa::~vvp_fun_edge_sa()
{
}

vthread_t vvp_fun_edge_sa::सङ्कलय_प्रतीक्षा_thread(vthread_t thread)
{
      vthread_t tmp = तन्तवः_2;
      तन्तवः_2 = thread;

      प्रत्यागम tmp;
}

शून्य vvp_fun_edge_sa::recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                                vvp_context_t)
{
      यदि (recv_vec4_(bit, द्वयङ्काः_2[पत्तनम्.पत्तनम्()], तन्तवः_2)) {
	    vvp_net_t*net = पत्तनम्.ptr();
	    net->send_vec4(bit, 0);
      }
}

शून्य vvp_fun_edge_sa::recv_vec4_pv(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
				   अचिह्नित आधारः, अचिह्नित vwid, vvp_context_t)
{
      प्रतिज्ञा(आधारः == 0);
      यदि (recv_vec4_(bit, द्वयङ्काः_2[पत्तनम्.पत्तनम्()], तन्तवः_2)) {
	    vvp_net_t*net = पत्तनम्.ptr();
	    net->send_vec4_pv(bit, आधारः, vwid, 0);
      }
}

vvp_fun_edge_aa::vvp_fun_edge_aa(edge_t e)
: vvp_fun_edge(e)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_सङ्कलय_item_गन्तव्यम्_context(एतत्, context_scope_);
}

vvp_fun_edge_aa::~vvp_fun_edge_aa()
{
}

शून्य vvp_fun_edge_aa::alloc_instance(vvp_context_t context)
{
      vvp_समुच्चयः_context_item(context, context_idx_, नूतन vvp_fun_edge_state_s);
      reset_instance(context);
}

शून्य vvp_fun_edge_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_edge_state_s*state = static_cast<vvp_fun_edge_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->तन्तवः = 0;
      प्रति (अचिह्नित idx = 0 ;  idx < 4 ;  idx += 1)
            state->द्वयङ्काः[idx] = द्वयङ्काः_2[idx];
}

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
शून्य vvp_fun_edge_aa::free_instance(vvp_context_t context)
{
      vvp_fun_edge_state_s*state = static_cast<vvp_fun_edge_state_s*>
            (vvp_get_context_item(context, context_idx_));
      विलोपय state;
}
#यदि_अन्तः

vthread_t vvp_fun_edge_aa::सङ्कलय_प्रतीक्षा_thread(vthread_t thread)
{
      vvp_fun_edge_state_s*state = static_cast<vvp_fun_edge_state_s*>
            (vthread_get_wt_context_item(context_idx_));

      vthread_t tmp = state->तन्तवः;
      state->तन्तवः = thread;

      प्रत्यागम tmp;
}

शून्य vvp_fun_edge_aa::recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                                vvp_context_t context)
{
      यदि (context) {
            vvp_fun_edge_state_s*state = static_cast<vvp_fun_edge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            यदि (recv_vec4_(bit, state->द्वयङ्काः[पत्तनम्.पत्तनम्()], state->तन्तवः)) {
                  vvp_net_t*net = पत्तनम्.ptr();
                  net->send_vec4(bit, context);
            }
      } अन्यथा {
            context = context_scope_->जीवितम्_contexts;
            यावत् (context) {
                  recv_vec4(पत्तनम्, bit, context);
                  context = vvp_get_अग्र_context(context);
            }
            द्वयङ्काः_2[पत्तनम्.पत्तनम्()] = bit.value(0);
      }
}

वर्ग anyedge_मानम् {

    सार्वजनिक:
      anyedge_मानम्() {};
      आभास ~anyedge_मानम्() {};

      आभास शून्य reset() = 0;

      आभास शून्य duplicate(anyedge_मानम्*&dup) = 0;
};

वर्ग anyedge_vec4_मानम् : सार्वजनिक anyedge_मानम् {

    सार्वजनिक:
      anyedge_vec4_मानम्() {};
      आभास ~anyedge_vec4_मानम्() override {};

      शून्य reset() override { old_द्वयङ्काः.समुच्चयः_गन्तव्यम्_x(); }

      शून्य समुच्चयः(ध्रुव vvp_vector4_t&bit) { old_द्वयङ्काः = bit; };

      शून्य duplicate(anyedge_मानम्*&dup) override;

      तार्किक recv_vec4(ध्रुव vvp_vector4_t&bit);

      तार्किक recv_vec4_pv(ध्रुव vvp_vector4_t&bit, अचिह्नित आधारः,
			अचिह्नित vwid);

    गुप्त:
      vvp_vector4_t old_द्वयङ्काः;
};

स्थिर anyedge_vec4_मानम्*get_vec4_मानम्(anyedge_मानम्*&value)
{
      anyedge_vec4_मानम्*vec4_मानम् = dynamic_cast<anyedge_vec4_मानम्*>(value);
      यदि (!value) {
	    vec4_मानम् = नूतन anyedge_vec4_मानम्();
	    विलोपय value;
	    value = vec4_मानम्;
      }
      प्रत्यागम vec4_मानम्;
}

वर्ग anyedge_real_मानम् : सार्वजनिक anyedge_मानम् {

    सार्वजनिक:
      anyedge_real_मानम्() : old_द्वयङ्काः(0.0) {};
      आभास ~anyedge_real_मानम्() override {};

      शून्य reset() override { old_द्वयङ्काः = 0.0; }

      शून्य समुच्चयः(द्विगुण bit) { old_द्वयङ्काः = bit; };

      शून्य duplicate(anyedge_मानम्*&dup) override;

      तार्किक recv_real(द्विगुण bit);

    गुप्त:
      द्विगुण old_द्वयङ्काः;
};

स्थिर anyedge_real_मानम्*get_real_मानम्(anyedge_मानम्*&value)
{
      anyedge_real_मानम्*real_मानम् = dynamic_cast<anyedge_real_मानम्*>(value);
      यदि (!value) {
	    real_मानम् = नूतन anyedge_real_मानम्();
	    विलोपय value;
	    value = real_मानम्;
      }
      प्रत्यागम real_मानम्;
}

वर्ग anyedge_string_मानम् : सार्वजनिक anyedge_मानम् {

    सार्वजनिक:
      anyedge_string_मानम्() {};
      आभास ~anyedge_string_मानम्() override {};

      शून्य reset() override { old_द्वयङ्काः.clear(); }

      शून्य समुच्चयः(ध्रुव std::string&bit) { old_द्वयङ्काः = bit; };

      शून्य duplicate(anyedge_मानम्*&dup) override;

      तार्किक recv_string(ध्रुव std::string&bit);

    गुप्त:
      std::string old_द्वयङ्काः;
};

स्थिर anyedge_string_मानम्*get_string_मानम्(anyedge_मानम्*&value)
{
      anyedge_string_मानम्*string_मानम् = dynamic_cast<anyedge_string_मानम्*>(value);
      यदि (!value) {
	    string_मानम् = नूतन anyedge_string_मानम्();
	    विलोपय value;
	    value = string_मानम्;
      }
      प्रत्यागम string_मानम्;
}

संरचना vvp_fun_anyedge_state_s : सार्वजनिक waitable_state_s {
      vvp_fun_anyedge_state_s()
      {
            प्रति (अचिह्नित idx = 0 ;  idx < 4 ;  idx += 1)
                  चरमः_मानम्[idx] = 0;
      }

      ~vvp_fun_anyedge_state_s()
      {
            प्रति (अचिह्नित idx = 0 ;  idx < 4 ;  idx += 1)
                  विलोपय चरमः_मानम्[idx];
      }

      anyedge_मानम् *चरमः_मानम्[4];
      vvp_fun_anyedge_state_s(ध्रुव vvp_fun_anyedge_state_s&) = विलोपय;
      vvp_fun_anyedge_state_s& संक्रियकः=(ध्रुव vvp_fun_anyedge_state_s&) = विलोपय;
};

vvp_fun_anyedge::vvp_fun_anyedge()
{
      प्रति (अचिह्नित idx = 0 ;  idx < 4 ;  idx += 1)
	    चरमः_मानम्[idx] = 0;
}

vvp_fun_anyedge::~vvp_fun_anyedge()
{
      प्रति (अचिह्नित idx = 0 ;  idx < 4 ;  idx += 1)
	    विलोपय चरमः_मानम्[idx];
}

शून्य anyedge_vec4_मानम्::duplicate(anyedge_मानम्*&dup)
{
      anyedge_vec4_मानम्*dup_vec4 = get_vec4_मानम्(dup);
      प्रतिज्ञा(dup_vec4);
      dup_vec4->समुच्चयः(old_द्वयङ्काः);
}

तार्किक anyedge_vec4_मानम्::recv_vec4(ध्रुव vvp_vector4_t&bit)
{
      तार्किक flag = असत्य;

      यदि (old_द्वयङ्काः.size() != bit.size()) {
	    यदि (old_द्वयङ्काः.size() == 0) {
		    // Special case: If we've not seen any input yet
		    // (old_bits.size()==0) then replace it will a reference
		    // vector that is 'bx. Then compare that with the input
		    // to see if we are processing a change from 'bx.
		  old_द्वयङ्काः = vvp_vector4_t(bit.size(), BIT4_X);
		  यदि (old_द्वयङ्काः.eeq(bit))
			flag = असत्य;
		  अन्यथा
			flag = सत्य;

	    } अन्यथा {
		  flag = सत्य;
	    }

      } अन्यथा {
	    प्रति (अचिह्नित idx = 0 ;  idx < bit.size() ;  idx += 1) {
		  यदि (old_द्वयङ्काः.value(idx) != bit.value(idx)) {
			flag = सत्य;
			भङ्ग;
		  }
	    }
      }

      यदि (flag) {
	    old_द्वयङ्काः = bit;
      }

      प्रत्यागम flag;
}

तार्किक anyedge_vec4_मानम्::recv_vec4_pv(ध्रुव vvp_vector4_t&bit, अचिह्नित आधारः,
				      अचिह्नित vwid)
{
      vvp_vector4_t tmp = old_द्वयङ्काः;
      यदि (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid, BIT4_Z);
      प्रतिज्ञा(आधारः + bit.size()<= vwid);
      प्रतिज्ञा(tmp.size() == vwid);
      tmp.समुच्चयः_vec(आधारः, bit);

      प्रत्यागम recv_vec4(tmp);
}

शून्य anyedge_real_मानम्::duplicate(anyedge_मानम्*&dup)
{
      anyedge_real_मानम्*dup_real = get_real_मानम्(dup);
      प्रतिज्ञा(dup_real);
      dup_real->समुच्चयः(old_द्वयङ्काः);
}

तार्किक anyedge_real_मानम्::recv_real(द्विगुण bit)
{
      यदि (old_द्वयङ्काः != bit) {
	    old_द्वयङ्काः = bit;
            प्रत्यागम सत्य;
      }
      प्रत्यागम असत्य;
}

शून्य anyedge_string_मानम्::duplicate(anyedge_मानम्*&dup)
{
      anyedge_string_मानम्*dup_string = get_string_मानम्(dup);
      प्रतिज्ञा(dup_string);
      dup_string->समुच्चयः(old_द्वयङ्काः);
}

तार्किक anyedge_string_मानम्::recv_string(ध्रुव std::string&bit)
{
      यदि (old_द्वयङ्काः != bit) {
	    old_द्वयङ्काः = bit;
            प्रत्यागम सत्य;
      }
      प्रत्यागम असत्य;
}

vvp_fun_anyedge_sa::vvp_fun_anyedge_sa()
: तन्तवः_2(0)
{
}

vvp_fun_anyedge_sa::~vvp_fun_anyedge_sa()
{
}

vthread_t vvp_fun_anyedge_sa::सङ्कलय_प्रतीक्षा_thread(vthread_t thread)
{
      vthread_t tmp = तन्तवः_2;
      तन्तवः_2 = thread;

      प्रत्यागम tmp;
}

शून्य vvp_fun_anyedge_sa::recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                                   vvp_context_t)
{
      anyedge_vec4_मानम्*value = get_vec4_मानम्(चरमः_मानम्[पत्तनम्.पत्तनम्()]);
      प्रतिज्ञा(value);
      यदि (value->recv_vec4(bit)) {
	    सततम्_प्रतीक्षा_तन्तवः(तन्तवः_2);
	    vvp_net_t*net = पत्तनम्.ptr();
	    net->send_vec4(bit, 0);
      }
}

शून्य vvp_fun_anyedge_sa::recv_vec4_pv(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
				      अचिह्नित आधारः, अचिह्नित vwid, vvp_context_t)
{
      anyedge_vec4_मानम्*value = get_vec4_मानम्(चरमः_मानम्[पत्तनम्.पत्तनम्()]);
      प्रतिज्ञा(value);
      यदि (value->recv_vec4_pv(bit, आधारः, vwid)) {
	    सततम्_प्रतीक्षा_तन्तवः(तन्तवः_2);
	    vvp_net_t*net = पत्तनम्.ptr();
	    net->send_vec4(bit, 0);
      }
}

शून्य vvp_fun_anyedge_sa::recv_real(vvp_net_ptr_t पत्तनम्, द्विगुण bit,
                                   vvp_context_t)
{
      anyedge_real_मानम्*value = get_real_मानम्(चरमः_मानम्[पत्तनम्.पत्तनम्()]);
      प्रतिज्ञा(value);
      यदि (value->recv_real(bit)) {
	    सततम्_प्रतीक्षा_तन्तवः(तन्तवः_2);
	    vvp_net_t*net = पत्तनम्.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

शून्य vvp_fun_anyedge_sa::recv_string(vvp_net_ptr_t पत्तनम्, ध्रुव std::string&bit,
				     vvp_context_t)
{
      anyedge_string_मानम्*value = get_string_मानम्(चरमः_मानम्[पत्तनम्.पत्तनम्()]);
      प्रतिज्ञा(value);
      यदि (value->recv_string(bit)) {
	    सततम्_प्रतीक्षा_तन्तवः(तन्तवः_2);
	    vvp_net_t*net = पत्तनम्.ptr();
	    net->send_vec4(vvp_vector4_t(), 0);
      }
}

/*
 * An anyedge receiving an object should do nothing with it, but should
 * trigger waiting threads.
 */
शून्य vvp_fun_anyedge_sa::recv_object(vvp_net_ptr_t पत्तनम्, vvp_object_t,
				     vvp_context_t)
{
      सततम्_प्रतीक्षा_तन्तवः(तन्तवः_2);
      vvp_net_t*net = पत्तनम्.ptr();
      net->send_vec4(vvp_vector4_t(), 0);
}

vvp_fun_anyedge_aa::vvp_fun_anyedge_aa()
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_सङ्कलय_item_गन्तव्यम्_context(एतत्, context_scope_);
}

vvp_fun_anyedge_aa::~vvp_fun_anyedge_aa()
{
}

शून्य vvp_fun_anyedge_aa::alloc_instance(vvp_context_t context)
{
      vvp_समुच्चयः_context_item(context, context_idx_, नूतन vvp_fun_anyedge_state_s);
      reset_instance(context);
}

शून्य vvp_fun_anyedge_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->तन्तवः = 0;
      प्रति (अचिह्नित idx = 0 ;  idx < 4 ;  idx += 1) {
	    यदि (चरमः_मानम्[idx])
	          चरमः_मानम्[idx]->duplicate(state->चरमः_मानम्[idx]);
	    अन्यथा यदि (state->चरमः_मानम्[idx])
		  state->चरमः_मानम्[idx]->reset();
      }
}

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
शून्य vvp_fun_anyedge_aa::free_instance(vvp_context_t context)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vvp_get_context_item(context, context_idx_));
      विलोपय state;
}
#यदि_अन्तः

vthread_t vvp_fun_anyedge_aa::सङ्कलय_प्रतीक्षा_thread(vthread_t thread)
{
      vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
            (vthread_get_wt_context_item(context_idx_));

      vthread_t tmp = state->तन्तवः;
      state->तन्तवः = thread;

      प्रत्यागम tmp;
}

शून्य vvp_fun_anyedge_aa::recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      यदि (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_vec4_मानम्*value = get_vec4_मानम्(state->चरमः_मानम्[पत्तनम्.पत्तनम्()]);
            प्रतिज्ञा(value);
            यदि (value->recv_vec4(bit)) {
                  सततम्_प्रतीक्षा_तन्तवः(state->तन्तवः);
                  vvp_net_t*net = पत्तनम्.ptr();
                  net->send_vec4(bit, context);
            }
      } अन्यथा {
            context = context_scope_->जीवितम्_contexts;
            यावत् (context) {
                  recv_vec4(पत्तनम्, bit, context);
                  context = vvp_get_अग्र_context(context);
            }
            anyedge_vec4_मानम्*value = get_vec4_मानम्(चरमः_मानम्[पत्तनम्.पत्तनम्()]);
            प्रतिज्ञा(value);
            value->समुच्चयः(bit);
      }
}

शून्य vvp_fun_anyedge_aa::recv_real(vvp_net_ptr_t पत्तनम्, द्विगुण bit,
                                   vvp_context_t context)
{
      यदि (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_real_मानम्*value = get_real_मानम्(state->चरमः_मानम्[पत्तनम्.पत्तनम्()]);
            प्रतिज्ञा(value);
            यदि (value->recv_real(bit)) {
                  सततम्_प्रतीक्षा_तन्तवः(state->तन्तवः);
                  vvp_net_t*net = पत्तनम्.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } अन्यथा {
            context = context_scope_->जीवितम्_contexts;
            यावत् (context) {
                  recv_real(पत्तनम्, bit, context);
                  context = vvp_get_अग्र_context(context);
            }
            anyedge_real_मानम्*value = get_real_मानम्(चरमः_मानम्[पत्तनम्.पत्तनम्()]);
            प्रतिज्ञा(value);
            value->समुच्चयः(bit);
      }
}

शून्य vvp_fun_anyedge_aa::recv_string(vvp_net_ptr_t पत्तनम्, ध्रुव std::string&bit,
				     vvp_context_t context)
{
      यदि (context) {
            vvp_fun_anyedge_state_s*state = static_cast<vvp_fun_anyedge_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            anyedge_string_मानम्*value = get_string_मानम्(state->चरमः_मानम्[पत्तनम्.पत्तनम्()]);
            प्रतिज्ञा(value);
            यदि (value->recv_string(bit)) {
                  सततम्_प्रतीक्षा_तन्तवः(state->तन्तवः);
                  vvp_net_t*net = पत्तनम्.ptr();
                  net->send_vec4(vvp_vector4_t(), context);
            }
      } अन्यथा {
            context = context_scope_->जीवितम्_contexts;
            यावत् (context) {
                  recv_string(पत्तनम्, bit, context);
                  context = vvp_get_अग्र_context(context);
            }
            anyedge_string_मानम्*value = get_string_मानम्(चरमः_मानम्[पत्तनम्.पत्तनम्()]);
            प्रतिज्ञा(value);
            value->समुच्चयः(bit);
      }
}

vvp_fun_event_or::vvp_fun_event_or(vvp_net_t*आधारः_net)
: आधारः_net_2(आधारः_net)
{
}

vvp_fun_event_or::~vvp_fun_event_or()
{
}

vvp_fun_event_or_sa::vvp_fun_event_or_sa(vvp_net_t*आधारः_net)
: vvp_fun_event_or(आधारः_net), तन्तवः_2(0)
{
}

vvp_fun_event_or_sa::~vvp_fun_event_or_sa()
{
}

vthread_t vvp_fun_event_or_sa::सङ्कलय_प्रतीक्षा_thread(vthread_t thread)
{
      vthread_t tmp = तन्तवः_2;
      तन्तवः_2 = thread;

      प्रत्यागम tmp;
}

शून्य vvp_fun_event_or_sa::recv_vec4(vvp_net_ptr_t, ध्रुव vvp_vector4_t&bit,
                                    vvp_context_t)
{
      सततम्_प्रतीक्षा_तन्तवः(तन्तवः_2);
      आधारः_net_2->send_vec4(bit, 0);
}

vvp_fun_event_or_aa::vvp_fun_event_or_aa(vvp_net_t*आधारः_net)
: vvp_fun_event_or(आधारः_net)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_सङ्कलय_item_गन्तव्यम्_context(एतत्, context_scope_);
}

vvp_fun_event_or_aa::~vvp_fun_event_or_aa()
{
}

शून्य vvp_fun_event_or_aa::alloc_instance(vvp_context_t context)
{
      vvp_समुच्चयः_context_item(context, context_idx_, नूतन waitable_state_s);
}

शून्य vvp_fun_event_or_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->तन्तवः = 0;
}

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
शून्य vvp_fun_event_or_aa::free_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_item(context, context_idx_));
      विलोपय state;
}
#यदि_अन्तः

vthread_t vvp_fun_event_or_aa::सङ्कलय_प्रतीक्षा_thread(vthread_t thread)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vthread_get_wt_context_item(context_idx_));

      vthread_t tmp = state->तन्तवः;
      state->तन्तवः = thread;

      प्रत्यागम tmp;
}

शून्य vvp_fun_event_or_aa::recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                                    vvp_context_t context)
{
      यदि (context) {
            waitable_state_s*state = static_cast<waitable_state_s*>
                  (vvp_get_context_item(context, context_idx_));

            सततम्_प्रतीक्षा_तन्तवः(state->तन्तवः);
            आधारः_net_2->send_vec4(bit, context);
      } अन्यथा {
            context = context_scope_->जीवितम्_contexts;
            यावत् (context) {
                  recv_vec4(पत्तनम्, bit, context);
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
: vvp_named_event(h), तन्तवः_2(0)
{
}

vvp_named_event_sa::~vvp_named_event_sa()
{
}

vthread_t vvp_named_event_sa::सङ्कलय_प्रतीक्षा_thread(vthread_t thread)
{
      vthread_t tmp = तन्तवः_2;
      तन्तवः_2 = thread;

      प्रत्यागम tmp;
}

शून्य vvp_named_event_sa::recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                                   vvp_context_t)
{
      सततम्_प्रतीक्षा_तन्तवः(तन्तवः_2);
      vvp_net_t*net = पत्तनम्.ptr();
      net->send_vec4(bit, 0);

      __vpiNamedEvent*obj = dynamic_cast<__vpiNamedEvent*>(handle_);
      प्रतिज्ञा(obj);
      obj->सततम्_vpi_callbacks();
}

vvp_named_event_aa::vvp_named_event_aa(__vpiHandle*h)
: vvp_named_event(h)
{
      context_idx_ = vpip_सङ्कलय_item_गन्तव्यम्_context(एतत्, vpip_peek_context_scope());
}

vvp_named_event_aa::~vvp_named_event_aa()
{
}

शून्य vvp_named_event_aa::alloc_instance(vvp_context_t context)
{
      vvp_समुच्चयः_context_item(context, context_idx_, नूतन waitable_state_s);
}

शून्य vvp_named_event_aa::reset_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_item(context, context_idx_));

      state->तन्तवः = 0;
}

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
शून्य vvp_named_event_aa::free_instance(vvp_context_t context)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_item(context, context_idx_));
      विलोपय state;
}
#यदि_अन्तः

vthread_t vvp_named_event_aa::सङ्कलय_प्रतीक्षा_thread(vthread_t thread)
{
      waitable_state_s*state = static_cast<waitable_state_s*>
            (vthread_get_wt_context_item(context_idx_));

      vthread_t tmp = state->तन्तवः;
      state->तन्तवः = thread;

      प्रत्यागम tmp;
}

शून्य vvp_named_event_aa::recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      प्रतिज्ञा(context);

      waitable_state_s*state = static_cast<waitable_state_s*>
            (vvp_get_context_item(context, context_idx_));

      सततम्_प्रतीक्षा_तन्तवः(state->तन्तवः);
      vvp_net_t*net = पत्तनम्.ptr();
      net->send_vec4(bit, context);
}

/*
**  Create an event functor
**  edge:  compile_event(label, type, argc, argv, debug_flag)
**  or:    compile_event(label, NULL, argc, argv, debug_flag)
**
**  Named events are handled elsewhere.
*/

स्थिर शून्य compile_event_or(वर्ण*label, अचिह्नित argc, संरचना symb_s*argv);

शून्य compile_event(वर्ण*label, वर्ण*type, अचिह्नित argc, संरचना symb_s*argv)
{
      vvp_net_fun_t*fun = 0;

      यदि (type == 0) {
	    compile_event_or(label, argc, argv);
	    प्रत्यागम;
      }

      यदि (वाक्यं_तुलय(type,"anyedge") == 0) {

	    मुक्त(type);

            यदि (vpip_peek_current_scope()->is_automatic()) {
                  fun = नूतन vvp_fun_anyedge_aa;
            } अन्यथा {
                  fun = नूतन vvp_fun_anyedge_sa;
            }

      } अन्यथा {

	    vvp_fun_edge::edge_t edge_type = vvp_edge_none;

	    यदि (वाक्यं_तुलय(type,"posedge") == 0)
		  edge_type = vvp_edge_posedge;
	    अन्यथा यदि (वाक्यं_तुलय(type,"negedge") == 0)
		  edge_type = vvp_edge_negedge;
	    अन्यथा यदि (वाक्यं_तुलय(type,"edge") == 0)
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

      define_functor_चिह्नम्(label, ptr);
      मुक्त(label);

      inputs_connect(ptr, argc, argv);
      मुक्त(argv);
}

स्थिर शून्य compile_event_or(वर्ण*label, अचिह्नित argc, संरचना symb_s*argv)
{
      vvp_net_t*आधारः_net = नूतन vvp_net_t;
      यदि (vpip_peek_current_scope()->is_automatic()) {
            आधारः_net->fun = नूतन vvp_fun_event_or_aa(आधारः_net);
      } अन्यथा {
            आधारः_net->fun = नूतन vvp_fun_event_or_sa(आधारः_net);
      }
      define_functor_चिह्नम्(label, आधारः_net);
      मुक्त(label);

	/* This is a simplified version of a wide functor. We don't
	   care about the data values or what port they arrived on,
	   so we can use a single shared functor. */
      vvp_net_t*curr_net = आधारः_net;
      प्रति (अचिह्नित idx = 0 ;  idx < argc ;  idx += 1) {
	    यदि (idx > 0 && (idx % 4) == 0) {
		  curr_net = नूतन vvp_net_t;
		  curr_net->fun = आधारः_net->fun;
	    }
	    निवेशः_connect(curr_net, idx % 4, argv[idx].text);
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
      define_functor_चिह्नम्(label, ptr);
      compile_vpi_चिह्नम्(label, obj);
      यदि (! local_flag) vpip_attach_गन्तव्यम्_current_scope(obj);

      मुक्त(label);
      विलोपय[] name;
}

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
शून्य named_event_delete(__vpiHandle*handle)
{
      विलोपय dynamic_cast<__vpiNamedEvent *>(handle);
}
#यदि_अन्तः
