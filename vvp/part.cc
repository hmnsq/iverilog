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

ఉపయోగించి నామాకాశ std;

నిర్మాణం vvp_fun_part_state_s {
      vvp_fun_part_state_s() {}

      vvp_vector4_t బిట్లు;
};

vvp_fun_part::vvp_fun_part(గుర్తులేని పునాది, గుర్తులేని wid)
: పునాది_2(పునాది), wid_(wid)
{
}

vvp_fun_part::~vvp_fun_part()
{
}

vvp_fun_part_sa::vvp_fun_part_sa(గుర్తులేని పునాది, గుర్తులేని wid)
: vvp_fun_part(పునాది, wid)
{
      net_ = 0;
}

vvp_fun_part_sa::~vvp_fun_part_sa()
{
}

శూన్య vvp_fun_part_sa::recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                vvp_context_t)
{
      ప్రతిజ్ఞా(పోర్టు.పోర్టు() == 0);

      vvp_vector4_t tmp (బిట్టు, పునాది_2, wid_);
      ఒకవేళ (val_ .eeq( tmp ))
	    తిరిగి;

      val_ = tmp;

      ఒకవేళ (net_ == 0) {
	    net_ = పోర్టు.ptr();
	    కాలపట్టిక_functor(ఇది);
      }
}

/*
 * Handle the case that the part select node is actually fed by a part
 * select assignment. It's not exactly clear what might make this
 * happen, but is does seem to happen and this should have well
 * defined behavior.
 */
శూన్య vvp_fun_part_sa::recv_vec4_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
				   గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t)
{
      vvp_vector4_t tmp (vwid, BIT4_Z);
      tmp.సమితి_vec(పునాది_2, val_);
      tmp.సమితి_vec(పునాది, బిట్టు);
      recv_vec4(పోర్టు, tmp, 0);
}

శూన్య vvp_fun_part_sa::నిరంతరం_నిరంతరం()
{
      vvp_net_t*ptr = net_;
      net_ = 0;
      ptr->send_vec4(val_, 0);
}

vvp_fun_part_aa::vvp_fun_part_aa(గుర్తులేని పునాది, గుర్తులేని wid)
: vvp_fun_part(పునాది, wid)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_కూడు_అంశం_గమ్యం_context(ఇది, context_scope_);
}

vvp_fun_part_aa::~vvp_fun_part_aa()
{
}

శూన్య vvp_fun_part_aa::alloc_instance(vvp_context_t context)
{
      vvp_సమితి_context_అంశం(context, context_idx_, కొత్త vvp_vector4_t);
}

శూన్య vvp_fun_part_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_అంశం(context, context_idx_));

      val->సమితి_గమ్యం_x();
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య vvp_fun_part_aa::free_instance(vvp_context_t context)
{
      vvp_vector4_t*val = static_cast<vvp_vector4_t*>
            (vvp_get_context_అంశం(context, context_idx_));
      తొలగించు val;
}
#అయితే_ముగింపు

శూన్య vvp_fun_part_aa::recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                vvp_context_t context)
{
      ఒకవేళ (context) {
            ప్రతిజ్ఞా(పోర్టు.పోర్టు() == 0);

            vvp_vector4_t*val = static_cast<vvp_vector4_t*>
                  (vvp_get_context_అంశం(context, context_idx_));

            vvp_vector4_t tmp (wid_, BIT4_X);
            కొరకు (గుర్తులేని idx = 0 ;  idx < wid_ ;  idx += 1) {
                  ఒకవేళ (idx + పునాది_2 < బిట్టు.size())
                        tmp.సమితి_బిట్టు(idx, బిట్టు.value(పునాది_2+idx));
            }
            ఒకవేళ (!val->eeq( tmp )) {
                  *val = tmp;
                  పోర్టు.ptr()->send_vec4(tmp, context);
            }
      } లేకపోతే {
            context = context_scope_->సజీవ_contexts;
            అయితే (context) {
                  recv_vec4(పోర్టు, బిట్టు, context);
                  context = vvp_get_తదుపరి_context(context);
            }
      }
}

/*
 * Handle the case that the part select node is actually fed by a part
 * select assignment. It's not exactly clear what might make this
 * happen, but is does seem to happen and this should have well
 * defined behavior.
 */
శూన్య vvp_fun_part_aa::recv_vec4_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
				   గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t context)
{
      ఒకవేళ (context) {
	    స్థిరరాశి vvp_vector4_t*val = static_cast<vvp_vector4_t*>
	                                         (vvp_get_context_అంశం(context,
	                                                               context_idx_));

	    vvp_vector4_t tmp (vwid, BIT4_Z);
	    tmp.సమితి_vec(పునాది_2, *val);
	    tmp.సమితి_vec(పునాది, బిట్టు);
	    recv_vec4(పోర్టు, tmp, context);
      } లేకపోతే {
	    context = context_scope_->సజీవ_contexts;
	    అయితే (context) {
		  recv_vec4_pv(పోర్టు, బిట్టు, పునాది, vwid, context);
		  context = vvp_get_తదుపరి_context(context);
	    }
      }
}

vvp_fun_part_pv::vvp_fun_part_pv(గుర్తులేని b, గుర్తులేని w, గుర్తులేని v)
: పునాది_2(b), wid_(w), vwid_(v)
{
}

vvp_fun_part_pv::~vvp_fun_part_pv()
{
}

శూన్య vvp_fun_part_pv::recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                vvp_context_t context)
{
      ప్రతిజ్ఞా(పోర్టు.పోర్టు() == 0);

      ఒకవేళ (బిట్టు.size() != wid_) {
	    cerr << "internal error: part_pv data mismatch. "
		 << "base_=" << పునాది_2 << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << బిట్టు
		 << పంక్తి_ముగింపు;
      }
      ప్రతిజ్ఞా(బిట్టు.size() == wid_);

      పోర్టు.ptr()->send_vec4_pv(బిట్టు, పునాది_2, vwid_, context);
}

శూన్య vvp_fun_part_pv::recv_vec4_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                 గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t ctx)
{
      ప్రతిజ్ఞా(పోర్టు.పోర్టు() == 0);
      ప్రతిజ్ఞా(పునాది + బిట్టు.size() <= vwid);
      ప్రతిజ్ఞా(vwid == wid_);

      vvp_vector4_t tmp(wid_, BIT4_Z);
      tmp.సమితి_vec(పునాది, బిట్టు);

      పోర్టు.ptr()->send_vec4_pv(tmp, పునాది_2, vwid_, ctx);
}

శూన్య vvp_fun_part_pv::recv_vec8(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector8_t&బిట్టు)
{
      ప్రతిజ్ఞా(పోర్టు.పోర్టు() == 0);

      ఒకవేళ (బిట్టు.size() != wid_) {
	    cerr << "internal error: part_pv (strength-aware) data mismatch. "
		 << "base_=" << పునాది_2 << ", wid_=" << wid_
		 << ", vwid_=" << vwid_ << ", bit=" << బిట్టు
		 << పంక్తి_ముగింపు;
      }
      ప్రతిజ్ఞా(బిట్టు.size() == wid_);

      పోర్టు.ptr()->send_vec8_pv(బిట్టు, పునాది_2, vwid_);
}

vvp_fun_part_var::vvp_fun_part_var(గుర్తులేని w, తార్కిక is_signed)
: wid_(w), is_signed_(is_signed)
{
}

vvp_fun_part_var::~vvp_fun_part_var()
{
}

తార్కిక vvp_fun_part_var::recv_vec4_(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                  పూర్ణాంకం&పునాది, vvp_vector4_t&ఆధారం,
                                  vvp_vector4_t&ref)
{
      int32_t tmp;
      ఎంపిక (పోర్టు.పోర్టు()) {
	  స్థితి 0:
	    ఆధారం = బిట్టు;
	    ఆపు;
	  స్థితి 1:
	      // INT_MIN is before the vector and is used to
	      // represent a 'bx value on the select input.
	      // It is also used to guard against 32 bit
	      // unsigned -> signed address overflow / wrap around.
	    ఒకవేళ (!vector4_గమ్యం_విలువ(బిట్టు, tmp, is_signed_) || (!is_signed_ && tmp < 0))
		  tmp = INT32_MIN;
	    ఒకవేళ (static_cast<పూర్ణాంకం>(tmp) == పునాది) తిరిగి అబద్ధం;
	    పునాది = tmp;
	    ఆపు;
	  అప్రమేయ:
	    fprintf(stderr, "Unsupported port type %u.\n", పోర్టు.పోర్టు());
	    ప్రతిజ్ఞా(0);
	    ఆపు;
      }

      vvp_vector4_t res (wid_);

      కొరకు (గుర్తులేని idx = 0 ;  idx < wid_ ;  idx += 1) {
	    పూర్ణాంకం adr = పునాది+idx;
	    ఒకవేళ (adr < 0) కొనసాగు;
	    ఒకవేళ ((గుర్తులేని)adr >= ఆధారం.size()) ఆపు;

	    res.సమితి_బిట్టు(idx, ఆధారం.value((గుర్తులేని)adr));
      }

      ఒకవేళ (! ref.eeq(res)) {
	    ref = res;
            తిరిగి నిజం;
      }
      తిరిగి అబద్ధం;
}

vvp_fun_part_var_sa::vvp_fun_part_var_sa(గుర్తులేని w, తార్కిక is_signed)
: vvp_fun_part_var(w, is_signed), పునాది_2(0)
{
}

vvp_fun_part_var_sa::~vvp_fun_part_var_sa()
{
}

శూన్య vvp_fun_part_var_sa::recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                    vvp_context_t)
{
      ఒకవేళ (recv_vec4_(పోర్టు, బిట్టు, పునాది_2, ఆధారం_2, ref_)) {
	    పోర్టు.ptr()->send_vec4(ref_, 0);
      }
}

శూన్య vvp_fun_part_var_sa::recv_vec4_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
				       గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t)
{
      vvp_vector4_t tmp = ఆధారం_2;
      ఒకవేళ (tmp.size() == 0)
	    tmp = vvp_vector4_t(vwid);

      ప్రతిజ్ఞా(tmp.size() == vwid);
      tmp.సమితి_vec(పునాది, బిట్టు);
      recv_vec4(పోర్టు, tmp, 0);
}

నిర్మాణం vvp_fun_part_var_state_s {
      vvp_fun_part_var_state_s() : పునాది(0) { }

      పూర్ణాంకం పునాది;
      vvp_vector4_t ఆధారం;
      vvp_vector4_t ref;
};

vvp_fun_part_var_aa::vvp_fun_part_var_aa(గుర్తులేని w, తార్కిక is_signed)
: vvp_fun_part_var(w, is_signed)
{
      context_scope_ = vpip_peek_context_scope();
      context_idx_ = vpip_కూడు_అంశం_గమ్యం_context(ఇది, context_scope_);
}

vvp_fun_part_var_aa::~vvp_fun_part_var_aa()
{
}

శూన్య vvp_fun_part_var_aa::alloc_instance(vvp_context_t context)
{
      vvp_సమితి_context_అంశం(context, context_idx_, కొత్త vvp_fun_part_var_state_s);
}

శూన్య vvp_fun_part_var_aa::reset_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_అంశం(context, context_idx_));

      state->పునాది = 0;
      state->ఆధారం.సమితి_గమ్యం_x();
      state->ref.సమితి_గమ్యం_x();
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య vvp_fun_part_var_aa::free_instance(vvp_context_t context)
{
      vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
            (vvp_get_context_అంశం(context, context_idx_));
      తొలగించు state;
}
#అయితే_ముగింపు

శూన్య vvp_fun_part_var_aa::recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                    vvp_context_t context)
{
      ఒకవేళ (context) {
            vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
                  (vvp_get_context_అంశం(context, context_idx_));

            ఒకవేళ (recv_vec4_(పోర్టు, బిట్టు, state->పునాది, state->ఆధారం, state->ref)) {
                  పోర్టు.ptr()->send_vec4(state->ref, context);
            }
      } లేకపోతే {
            context = context_scope_->సజీవ_contexts;
            అయితే (context) {
                  recv_vec4(పోర్టు, బిట్టు, context);
                  context = vvp_get_తదుపరి_context(context);
            }
      }
}

శూన్య vvp_fun_part_var_aa::recv_vec4_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
				       గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t context)
{
      ఒకవేళ (context) {
	    స్థిరరాశి vvp_fun_part_var_state_s*state = static_cast<vvp_fun_part_var_state_s*>
	                                                      (vvp_get_context_అంశం(context,
	                                                       context_idx_));

            vvp_vector4_t tmp = state->ఆధారం;
            ఒకవేళ (tmp.size() == 0)
                  tmp = vvp_vector4_t(vwid);

            ప్రతిజ్ఞా(tmp.size() == vwid);
            tmp.సమితి_vec(పునాది, బిట్టు);
            recv_vec4(పోర్టు, tmp, context);
      } లేకపోతే {
            context = context_scope_->సజీవ_contexts;
            అయితే (context) {
                  recv_vec4(పోర్టు, బిట్టు, context);
                  context = vvp_get_తదుపరి_context(context);
            }
      }
}

/*
 * Given a node functor, create a network node and link it into the
 * netlist. This form assumes nodes with a single input.
 */
శూన్య link_కణుపు_1(అక్షరం*label, అక్షరం*ఆధారం, vvp_net_fun_t*fun)
{
      vvp_net_t*net = కొత్త vvp_net_t;
      net->fun = fun;

      define_functor_చిహ్నం(label, net);
      విడుదల(label);

      ఆదానం_connect(net, 0, ఆధారం);
}

శూన్య compile_part_select(అక్షరం*label, అక్షరం*ఆధారం,
			 గుర్తులేని పునాది, గుర్తులేని wid)
{
      vvp_fun_part*fun = 0;
      ఒకవేళ (vpip_peek_current_scope()->is_automatic()) {
            fun = కొత్త vvp_fun_part_aa(పునాది, wid);
      } లేకపోతే {
            fun = కొత్త vvp_fun_part_sa(పునాది, wid);
      }
      link_కణుపు_1(label, ఆధారం, fun);
}

శూన్య compile_part_select_pv(అక్షరం*label, అక్షరం*ఆధారం,
			    గుర్తులేని పునాది, గుర్తులేని wid,
			    గుర్తులేని సదిశ_wid)
{
      vvp_fun_part_pv*fun = కొత్త vvp_fun_part_pv(పునాది, wid, సదిశ_wid);
      link_కణుపు_1(label, ఆధారం, fun);
}

శూన్య compile_part_select_var(అక్షరం*label, అక్షరం*ఆధారం, అక్షరం*var,
			     గుర్తులేని wid, తార్కిక is_signed)
{
      vvp_fun_part_var*fun = 0;
      ఒకవేళ (vpip_peek_current_scope()->is_automatic()) {
            fun = కొత్త vvp_fun_part_var_aa(wid, is_signed);
      } లేకపోతే {
            fun = కొత్త vvp_fun_part_var_sa(wid, is_signed);
      }
      vvp_net_t*net = కొత్త vvp_net_t;
      net->fun = fun;

      define_functor_చిహ్నం(label, net);
      విడుదల(label);

      ఆదానం_connect(net, 0, ఆధారం);
      ఆదానం_connect(net, 1, var);
}
