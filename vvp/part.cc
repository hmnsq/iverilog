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

ورتوں نطاق std;

بنية vvp_fun_part_state_s {
      vvp_fun_part_state_s() {}

      vvp_vector4_t بٹ;
};

vvp_fun_part::vvp_fun_part(بنانشانی ادھار, بنانشانی wid)
: ادھار_2(ادھار), wid_(wid)
{
}

vvp_fun_part::~vvp_fun_part()
{
}

vvp_fun_part_sa::vvp_fun_part_sa(بنانشانی ادھار, بنانشانی wid)
: vvp_fun_part(ادھار, wid)
{
      net_ = 0;
}

vvp_fun_part_sa::~vvp_fun_part_sa()
{
}

فراغ vvp_fun_part_sa::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                vvp_context_t)
{
      أكد(پورٹ.پورٹ() == 0);

      vvp_vector4_t tmp (ثنائی_ہندسہ, ادھار_2, wid_);
      إذا (val_ .eeq( tmp ))
	    أرجع;

      val_ = tmp;

      إذا (net_ == 0) {
	    net_ = پورٹ.ptr();
	    ویلا_سارنی_functor(هذا);
      }
}

/*
 * Handle the case that the part select node is actually fed by a part
 * select assignment. It's not exactly clear what might make this
 * happen, but is does seem to happen and this should have well
 * defined behavior.
 */
فراغ vvp_fun_part_sa::recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				   بنانشانی ادھار, بنانشانی vwid, vvp_context_t)
{
      vvp_vector4_t tmp (vwid, BIT4_Z);
      tmp.سیٹ_vec(ادھار_2, val_);
      tmp.سیٹ_vec(ادھار, ثنائی_ہندسہ);
      recv_vec4(پورٹ, tmp, 0);
}

فراغ vvp_fun_part_sa::لگاتار_لگاتار()
{
      vvp_net_t*ptr = net_;
      net_ = 0;
      ptr->send_vec4(val_, 0);
}

vvp_fun_part_aa::vvp_fun_part_aa(بنانشانی ادھار, بنانشانی wid)
: vvp_fun_part(ادھار, wid)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_جوڑو_شے_منزل_context(هذا, context_scope_);
}

vvp_fun_part_aa::~vvp_fun_part_aa()
{
}

فراغ vvp_fun_part_aa::alloc_instance(vvp_context_t context)
{
      vvp_سیٹ_context_شے(context, context_idx_, جديد vvp_vector4_t);
}

فراغ vvp_fun_part_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_شے(context, context_idx_));

      val->سیٹ_منزل_x();
}

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
فراغ vvp_fun_part_aa::free_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_شے(context, context_idx_));
      مٹاؤ val;
}
#جے_انت

فراغ vvp_fun_part_aa::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                vvp_context_t context)
{
      إذا (context) {
            أكد(پورٹ.پورٹ() == 0);

            vvp_vector4_t*val = static_cast<vvp_vector4_t*>
                  (vvp_get_context_شے(context, context_idx_));

            vvp_vector4_t tmp (wid_, BIT4_X);
            لكل (بنانشانی idx = 0 ;  idx < wid_ ;  idx += 1) {
                  إذا (idx + ادھار_2 < ثنائی_ہندسہ.size())
                        tmp.سیٹ_ثنائی_ہندسہ(idx, ثنائی_ہندسہ.value(ادھار_2+idx));
            }
            إذا (!val->eeq( tmp )) {
                  *val = tmp;
                  پورٹ.ptr()->send_vec4(tmp, context);
            }
      } وإلا {
            context = context_scope_->جیوندا_contexts;
            طالما (context) {
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
فراغ vvp_fun_part_aa::recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				   بنانشانی ادھار, بنانشانی vwid, vvp_context_t context)
{
      إذا (context) {
	    ثابت vvp_vector4_t*val = static_cast<vvp_vector4_t*>
	                                         (vvp_get_context_شے(context,
	                                                               context_idx_));

	    vvp_vector4_t tmp (vwid, BIT4_Z);
	    tmp.سیٹ_vec(ادھار_2, *val);
	    tmp.سیٹ_vec(ادھار, ثنائی_ہندسہ);
	    recv_vec4(پورٹ, tmp, context);
      } وإلا {
	    context = context_scope_->جیوندا_contexts;
	    طالما (context) {
		  recv_vec4_pv(پورٹ, ثنائی_ہندسہ, ادھار, vwid, context);
		  context = vvp_get_اگلا_context(context);
	    }
      }
}

vvp_fun_part_pv::vvp_fun_part_pv(بنانشانی b, بنانشانی w, بنانشانی v)
: ادھار_2(b), wid_(w), vwid_(v)
{
}

vvp_fun_part_pv::~vvp_fun_part_pv()
{
}

فراغ vvp_fun_part_pv::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                vvp_context_t context)
{
      أكد(پورٹ.پورٹ() == 0);

      إذا (ثنائی_ہندسہ.size() != wid_) {
	    cerr << "internal error: part_pv data mismatch. "
		 << "base_=" << ادھار_2 << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << ثنائی_ہندسہ
		 << سطر_انت;
      }
      أكد(ثنائی_ہندسہ.size() == wid_);

      پورٹ.ptr()->send_vec4_pv(ثنائی_ہندسہ, ادھار_2, vwid_, context);
}

فراغ vvp_fun_part_pv::recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                 بنانشانی ادھار, بنانشانی vwid, vvp_context_t ctx)
{
      أكد(پورٹ.پورٹ() == 0);
      أكد(ادھار + ثنائی_ہندسہ.size() <= vwid);
      أكد(vwid == wid_);

      vvp_vector4_t tmp(wid_, BIT4_Z);
      tmp.سیٹ_vec(ادھار, ثنائی_ہندسہ);

      پورٹ.ptr()->send_vec4_pv(tmp, ادھار_2, vwid_, ctx);
}

فراغ vvp_fun_part_pv::recv_vec8(vvp_net_ptr_t پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ)
{
      أكد(پورٹ.پورٹ() == 0);

      إذا (ثنائی_ہندسہ.size() != wid_) {
	    cerr << "internal error: part_pv (strength-aware) data mismatch. "
		 << "base_=" << ادھار_2 << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << ثنائی_ہندسہ
		 << سطر_انت;
      }
      أكد(ثنائی_ہندسہ.size() == wid_);

      پورٹ.ptr()->send_vec8_pv(ثنائی_ہندسہ, ادھار_2, vwid_);
}

vvp_fun_part_var::vvp_fun_part_var(بنانشانی w, منطقي is_signed)
: wid_(w), is_signed_(is_signed)
{
}

vvp_fun_part_var::~vvp_fun_part_var()
{
}

منطقي vvp_fun_part_var::recv_vec4_(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                  صحيح&ادھار, vvp_vector4_t&سروت,
                                  vvp_vector4_t&ref)
{
      int32_t tmp;
      بدل (پورٹ.پورٹ()) {
	  حالة 0:
	    سروت = ثنائی_ہندسہ;
	    اكسر;
	  حالة 1:
	      // INT_MIN is before the vector and is used to
	      // represent a 'bx value on the select input.
	      // It is also used to guard against 32 bit
	      // unsigned -> signed address overflow / wrap around.
	    إذا (!vector4_منزل_قدر(ثنائی_ہندسہ, tmp, is_signed_) || (!is_signed_ && tmp < 0))
		  tmp = INT32_MIN;
	    إذا (static_cast<صحيح>(tmp) == ادھار) أرجع خطأ;
	    ادھار = tmp;
	    اكسر;
	  افتراضي:
	    fprintf(stderr, "Unsupported port type %u.\n", پورٹ.پورٹ());
	    أكد(0);
	    اكسر;
      }

      vvp_vector4_t res (wid_);

      لكل (بنانشانی idx = 0 ;  idx < wid_ ;  idx += 1) {
	    صحيح adr = ادھار+idx;
	    إذا (adr < 0) استمر;
	    إذا ((بنانشانی)adr >= سروت.size()) اكسر;

	    res.سیٹ_ثنائی_ہندسہ(idx, سروت.value((بنانشانی)adr));
      }

      إذا (! ref.eeq(res)) {
	    ref = res;
            أرجع سچ;
      }
      أرجع خطأ;
}

vvp_fun_part_var_sa::vvp_fun_part_var_sa(بنانشانی w, منطقي is_signed)
: vvp_fun_part_var(w, is_signed), ادھار_2(0)
{
}

vvp_fun_part_var_sa::~vvp_fun_part_var_sa()
{
}

فراغ vvp_fun_part_var_sa::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                    vvp_context_t)
{
      إذا (recv_vec4_(پورٹ, ثنائی_ہندسہ, ادھار_2, سروت_2, ref_)) {
	    پورٹ.ptr()->send_vec4(ref_, 0);
      }
}

فراغ vvp_fun_part_var_sa::recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				       بنانشانی ادھار, بنانشانی vwid, vvp_context_t)
{
      vvp_vector4_t tmp = سروت_2;
      إذا (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid);

      أكد(tmp.size() == vwid);
      tmp.سیٹ_vec(ادھار, ثنائی_ہندسہ);
      recv_vec4(پورٹ, tmp, 0);
}

بنية vvp_fun_part_var_state_s {
      vvp_fun_part_var_state_s() : ادھار(0) { }

      صحيح ادھار;
      vvp_vector4_t سروت;
      vvp_vector4_t ref;
};

vvp_fun_part_var_aa::vvp_fun_part_var_aa(بنانشانی w, منطقي is_signed)
: vvp_fun_part_var(w, is_signed)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_جوڑو_شے_منزل_context(هذا, context_scope_);
}

vvp_fun_part_var_aa::~vvp_fun_part_var_aa()
{
}

فراغ vvp_fun_part_var_aa::alloc_instance(vvp_context_t context)
{
      vvp_سیٹ_context_شے(context, context_idx_, جديد vvp_fun_part_var_state_s);
}

فراغ vvp_fun_part_var_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_شے(context, context_idx_));

      state->ادھار = 0;
      state->سروت.سیٹ_منزل_x();
      state->ref.سیٹ_منزل_x();
}

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
فراغ vvp_fun_part_var_aa::free_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_شے(context, context_idx_));
      مٹاؤ state;
}
#جے_انت

فراغ vvp_fun_part_var_aa::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                    vvp_context_t context)
{
      إذا (context) {
            vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
                  (vvp_get_context_شے(context, context_idx_));

            إذا (recv_vec4_(پورٹ, ثنائی_ہندسہ, state->ادھار, state->سروت, state->ref)) {
                  پورٹ.ptr()->send_vec4(state->ref, context);
            }
      } وإلا {
            context = context_scope_->جیوندا_contexts;
            طالما (context) {
                  recv_vec4(پورٹ, ثنائی_ہندسہ, context);
                  context = vvp_get_اگلا_context(context);
            }
      }
}

فراغ vvp_fun_part_var_aa::recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				       بنانشانی ادھار, بنانشانی vwid, vvp_context_t context)
{
      إذا (context) {
	    ثابت vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
	                                                      (vvp_get_context_شے(context,
	                                                       context_idx_));

            vvp_vector4_t tmp = state->سروت;
            إذا (tmp.size() == 0)
                  tmp = vvp_vector4_t(vwid);

            أكد(tmp.size() == vwid);
            tmp.سیٹ_vec(ادھار, ثنائی_ہندسہ);
            recv_vec4(پورٹ, tmp, context);
      } وإلا {
            context = context_scope_->جیوندا_contexts;
            طالما (context) {
                  recv_vec4(پورٹ, ثنائی_ہندسہ, context);
                  context = vvp_get_اگلا_context(context);
            }
      }
}

/*
 * Given a node functor, create a network node and link it into the
 * netlist. This form assumes nodes with a single input.
 */
فراغ link_گرہ_1(حرف*label, حرف*سروت, vvp_net_fun_t*fun)
{
      vvp_net_t*net = جديد vvp_net_t;
      net->fun = fun;

      define_functor_علامت(label, net);
      حرر(label);

      داخلہ_connect(net, 0, سروت);
}

فراغ compile_part_select(حرف*label, حرف*سروت,
			 بنانشانی ادھار, بنانشانی wid)
{
      vvp_fun_part*fun = 0;
      إذا (vpip_peek_current_scope()->is_automatic()) {
            fun = جديد vvp_fun_part_aa(ادھار, wid);
      } وإلا {
            fun = جديد vvp_fun_part_sa(ادھار, wid);
      }
      link_گرہ_1(label, سروت, fun);
}

فراغ compile_part_select_pv(حرف*label, حرف*سروت,
			    بنانشانی ادھار, بنانشانی wid,
			    بنانشانی سمتیہ_wid)
{
      vvp_fun_part_pv*fun = جديد vvp_fun_part_pv(ادھار, wid, سمتیہ_wid);
      link_گرہ_1(label, سروت, fun);
}

فراغ compile_part_select_var(حرف*label, حرف*سروت, حرف*var,
			     بنانشانی wid, منطقي is_signed)
{
      vvp_fun_part_var*fun = 0;
      إذا (vpip_peek_current_scope()->is_automatic()) {
            fun = جديد vvp_fun_part_var_aa(wid, is_signed);
      } وإلا {
            fun = جديد vvp_fun_part_var_sa(wid, is_signed);
      }
      vvp_net_t*net = جديد vvp_net_t;
      net->fun = fun;

      define_functor_علامت(label, net);
      حرر(label);

      داخلہ_connect(net, 0, سروت);
      داخلہ_connect(net, 1, var);
}
