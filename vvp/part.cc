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

استعمال نامخلا std;

ساخت vvp_fun_part_state_s {
      vvp_fun_part_state_s() {}

      vvp_vector4_t بٹ;
};

vvp_fun_part::vvp_fun_part(غیرعلامتی بنیاد, غیرعلامتی wid)
: بنیاد_2(بنیاد), wid_(wid)
{
}

vvp_fun_part::~vvp_fun_part()
{
}

vvp_fun_part_sa::vvp_fun_part_sa(غیرعلامتی بنیاد, غیرعلامتی wid)
: vvp_fun_part(بنیاد, wid)
{
      net_ = 0;
}

vvp_fun_part_sa::~vvp_fun_part_sa()
{
}

خالی vvp_fun_part_sa::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                vvp_context_t)
{
      أكد(پورٹ.پورٹ() == 0);

      vvp_vector4_t tmp (ثنائی_ہندسہ, بنیاد_2, wid_);
      اگر (val_ .eeq( tmp ))
	    واپس;

      val_ = tmp;

      اگر (net_ == 0) {
	    net_ = پورٹ.ptr();
	    جدول_وقت_functor(یہ);
      }
}

/*
 * Handle the case that the part select node is actually fed by a part
 * select assignment. It's not exactly clear what might make this
 * happen, but is does seem to happen and this should have well
 * defined behavior.
 */
خالی vvp_fun_part_sa::recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				   غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t)
{
      vvp_vector4_t tmp (vwid, BIT4_Z);
      tmp.سیٹ_vec(بنیاد_2, val_);
      tmp.سیٹ_vec(بنیاد, ثنائی_ہندسہ);
      recv_vec4(پورٹ, tmp, 0);
}

خالی vvp_fun_part_sa::تسلسل_تسلسل()
{
      vvp_net_t*ptr = net_;
      net_ = 0;
      ptr->send_vec4(val_, 0);
}

vvp_fun_part_aa::vvp_fun_part_aa(غیرعلامتی بنیاد, غیرعلامتی wid)
: vvp_fun_part(بنیاد, wid)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_جوڑو_شے_مقصد_context(یہ, context_scope_);
}

vvp_fun_part_aa::~vvp_fun_part_aa()
{
}

خالی vvp_fun_part_aa::alloc_instance(vvp_context_t context)
{
      vvp_سیٹ_context_شے(context, context_idx_, نیا vvp_vector4_t);
}

خالی vvp_fun_part_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_شے(context, context_idx_));

      val->سیٹ_مقصد_x();
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی vvp_fun_part_aa::free_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_شے(context, context_idx_));
      حذف val;
}
#اگر_ختم

خالی vvp_fun_part_aa::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                vvp_context_t context)
{
      اگر (context) {
            أكد(پورٹ.پورٹ() == 0);

            vvp_vector4_t*val = static_cast<vvp_vector4_t*>
                  (vvp_get_context_شے(context, context_idx_));

            vvp_vector4_t tmp (wid_, BIT4_X);
            برائے (غیرعلامتی idx = 0 ;  idx < wid_ ;  idx += 1) {
                  اگر (idx + بنیاد_2 < ثنائی_ہندسہ.size())
                        tmp.سیٹ_ثنائی_ہندسہ(idx, ثنائی_ہندسہ.value(بنیاد_2+idx));
            }
            اگر (!val->eeq( tmp )) {
                  *val = tmp;
                  پورٹ.ptr()->send_vec4(tmp, context);
            }
      } ورنہ {
            context = context_scope_->زندہ_contexts;
            جب (context) {
                  recv_vec4(پورٹ, ثنائی_ہندسہ, context);
                  context = vvp_get_اگلا_context(context);
            }
      }
}

/*
 * Handle the case that the part select node is actually fed by a part
 * select assignment. It's not exactly clear what might make this
 * happen, but is does seem to happen and this should have well
 * defined behavior.
 */
خالی vvp_fun_part_aa::recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				   غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t context)
{
      اگر (context) {
	    ثابت vvp_vector4_t*val = static_cast<vvp_vector4_t*>
	                                         (vvp_get_context_شے(context,
	                                                               context_idx_));

	    vvp_vector4_t tmp (vwid, BIT4_Z);
	    tmp.سیٹ_vec(بنیاد_2, *val);
	    tmp.سیٹ_vec(بنیاد, ثنائی_ہندسہ);
	    recv_vec4(پورٹ, tmp, context);
      } ورنہ {
	    context = context_scope_->زندہ_contexts;
	    جب (context) {
		  recv_vec4_pv(پورٹ, ثنائی_ہندسہ, بنیاد, vwid, context);
		  context = vvp_get_اگلا_context(context);
	    }
      }
}

vvp_fun_part_pv::vvp_fun_part_pv(غیرعلامتی b, غیرعلامتی w, غیرعلامتی v)
: بنیاد_2(b), wid_(w), vwid_(v)
{
}

vvp_fun_part_pv::~vvp_fun_part_pv()
{
}

خالی vvp_fun_part_pv::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                vvp_context_t context)
{
      أكد(پورٹ.پورٹ() == 0);

      اگر (ثنائی_ہندسہ.size() != wid_) {
	    cerr << "internal error: part_pv data mismatch. "
		 << "base_=" << بنیاد_2 << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << ثنائی_ہندسہ
		 << سطر_ختم;
      }
      أكد(ثنائی_ہندسہ.size() == wid_);

      پورٹ.ptr()->send_vec4_pv(ثنائی_ہندسہ, بنیاد_2, vwid_, context);
}

خالی vvp_fun_part_pv::recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                 غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t ctx)
{
      أكد(پورٹ.پورٹ() == 0);
      أكد(بنیاد + ثنائی_ہندسہ.size() <= vwid);
      أكد(vwid == wid_);

      vvp_vector4_t tmp(wid_, BIT4_Z);
      tmp.سیٹ_vec(بنیاد, ثنائی_ہندسہ);

      پورٹ.ptr()->send_vec4_pv(tmp, بنیاد_2, vwid_, ctx);
}

خالی vvp_fun_part_pv::recv_vec8(vvp_net_ptr_t پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ)
{
      أكد(پورٹ.پورٹ() == 0);

      اگر (ثنائی_ہندسہ.size() != wid_) {
	    cerr << "internal error: part_pv (strength-aware) data mismatch. "
		 << "base_=" << بنیاد_2 << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << ثنائی_ہندسہ
		 << سطر_ختم;
      }
      أكد(ثنائی_ہندسہ.size() == wid_);

      پورٹ.ptr()->send_vec8_pv(ثنائی_ہندسہ, بنیاد_2, vwid_);
}

vvp_fun_part_var::vvp_fun_part_var(غیرعلامتی w, منطقی is_signed)
: wid_(w), is_signed_(is_signed)
{
}

vvp_fun_part_var::~vvp_fun_part_var()
{
}

منطقی vvp_fun_part_var::recv_vec4_(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                  صحیح&بنیاد, vvp_vector4_t&ماخذ,
                                  vvp_vector4_t&ref)
{
      int32_t tmp;
      انتخاب (پورٹ.پورٹ()) {
	  حالت 0:
	    ماخذ = ثنائی_ہندسہ;
	    توڑو;
	  حالت 1:
	      // INT_MIN is before the vector and is used to
	      // represent a 'bx value on the select input.
	      // It is also used to guard against 32 bit
	      // unsigned -> signed address overflow / wrap around.
	    اگر (!vector4_مقصد_قدر(ثنائی_ہندسہ, tmp, is_signed_) || (!is_signed_ && tmp < 0))
		  tmp = INT32_MIN;
	    اگر (static_cast<صحیح>(tmp) == بنیاد) واپس جھوٹ;
	    بنیاد = tmp;
	    توڑو;
	  طےشدہ:
	    fprintf(stderr, "Unsupported port type %u.\n", پورٹ.پورٹ());
	    أكد(0);
	    توڑو;
      }

      vvp_vector4_t res (wid_);

      برائے (غیرعلامتی idx = 0 ;  idx < wid_ ;  idx += 1) {
	    صحیح adr = بنیاد+idx;
	    اگر (adr < 0) جاری;
	    اگر ((غیرعلامتی)adr >= ماخذ.size()) توڑو;

	    res.سیٹ_ثنائی_ہندسہ(idx, ماخذ.value((غیرعلامتی)adr));
      }

      اگر (! ref.eeq(res)) {
	    ref = res;
            واپس سچ;
      }
      واپس جھوٹ;
}

vvp_fun_part_var_sa::vvp_fun_part_var_sa(غیرعلامتی w, منطقی is_signed)
: vvp_fun_part_var(w, is_signed), بنیاد_2(0)
{
}

vvp_fun_part_var_sa::~vvp_fun_part_var_sa()
{
}

خالی vvp_fun_part_var_sa::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                    vvp_context_t)
{
      اگر (recv_vec4_(پورٹ, ثنائی_ہندسہ, بنیاد_2, ماخذ_2, ref_)) {
	    پورٹ.ptr()->send_vec4(ref_, 0);
      }
}

خالی vvp_fun_part_var_sa::recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				       غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t)
{
      vvp_vector4_t tmp = ماخذ_2;
      اگر (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid);

      أكد(tmp.size() == vwid);
      tmp.سیٹ_vec(بنیاد, ثنائی_ہندسہ);
      recv_vec4(پورٹ, tmp, 0);
}

ساخت vvp_fun_part_var_state_s {
      vvp_fun_part_var_state_s() : بنیاد(0) { }

      صحیح بنیاد;
      vvp_vector4_t ماخذ;
      vvp_vector4_t ref;
};

vvp_fun_part_var_aa::vvp_fun_part_var_aa(غیرعلامتی w, منطقی is_signed)
: vvp_fun_part_var(w, is_signed)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_جوڑو_شے_مقصد_context(یہ, context_scope_);
}

vvp_fun_part_var_aa::~vvp_fun_part_var_aa()
{
}

خالی vvp_fun_part_var_aa::alloc_instance(vvp_context_t context)
{
      vvp_سیٹ_context_شے(context, context_idx_, نیا vvp_fun_part_var_state_s);
}

خالی vvp_fun_part_var_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_شے(context, context_idx_));

      state->بنیاد = 0;
      state->ماخذ.سیٹ_مقصد_x();
      state->ref.سیٹ_مقصد_x();
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی vvp_fun_part_var_aa::free_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_شے(context, context_idx_));
      حذف state;
}
#اگر_ختم

خالی vvp_fun_part_var_aa::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                    vvp_context_t context)
{
      اگر (context) {
            vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
                  (vvp_get_context_شے(context, context_idx_));

            اگر (recv_vec4_(پورٹ, ثنائی_ہندسہ, state->بنیاد, state->ماخذ, state->ref)) {
                  پورٹ.ptr()->send_vec4(state->ref, context);
            }
      } ورنہ {
            context = context_scope_->زندہ_contexts;
            جب (context) {
                  recv_vec4(پورٹ, ثنائی_ہندسہ, context);
                  context = vvp_get_اگلا_context(context);
            }
      }
}

خالی vvp_fun_part_var_aa::recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				       غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t context)
{
      اگر (context) {
	    ثابت vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
	                                                      (vvp_get_context_شے(context,
	                                                       context_idx_));

            vvp_vector4_t tmp = state->ماخذ;
            اگر (tmp.size() == 0)
                  tmp = vvp_vector4_t(vwid);

            أكد(tmp.size() == vwid);
            tmp.سیٹ_vec(بنیاد, ثنائی_ہندسہ);
            recv_vec4(پورٹ, tmp, context);
      } ورنہ {
            context = context_scope_->زندہ_contexts;
            جب (context) {
                  recv_vec4(پورٹ, ثنائی_ہندسہ, context);
                  context = vvp_get_اگلا_context(context);
            }
      }
}

/*
 * Given a node functor, create a network node and link it into the
 * netlist. This form assumes nodes with a single input.
 */
خالی link_گرہ_1(حرف*label, حرف*ماخذ, vvp_net_fun_t*fun)
{
      vvp_net_t*net = نیا vvp_net_t;
      net->fun = fun;

      define_functor_علامت(label, net);
      آزاد(label);

      داخلہ_connect(net, 0, ماخذ);
}

خالی compile_part_select(حرف*label, حرف*ماخذ,
			 غیرعلامتی بنیاد, غیرعلامتی wid)
{
      vvp_fun_part*fun = 0;
      اگر (vpip_peek_current_scope()->is_automatic()) {
            fun = نیا vvp_fun_part_aa(بنیاد, wid);
      } ورنہ {
            fun = نیا vvp_fun_part_sa(بنیاد, wid);
      }
      link_گرہ_1(label, ماخذ, fun);
}

خالی compile_part_select_pv(حرف*label, حرف*ماخذ,
			    غیرعلامتی بنیاد, غیرعلامتی wid,
			    غیرعلامتی سمتیہ_wid)
{
      vvp_fun_part_pv*fun = نیا vvp_fun_part_pv(بنیاد, wid, سمتیہ_wid);
      link_گرہ_1(label, ماخذ, fun);
}

خالی compile_part_select_var(حرف*label, حرف*ماخذ, حرف*var,
			     غیرعلامتی wid, منطقی is_signed)
{
      vvp_fun_part_var*fun = 0;
      اگر (vpip_peek_current_scope()->is_automatic()) {
            fun = نیا vvp_fun_part_var_aa(wid, is_signed);
      } ورنہ {
            fun = نیا vvp_fun_part_var_sa(wid, is_signed);
      }
      vvp_net_t*net = نیا vvp_net_t;
      net->fun = fun;

      define_functor_علامت(label, net);
      آزاد(label);

      داخلہ_connect(net, 0, ماخذ);
      داخلہ_connect(net, 1, var);
}
