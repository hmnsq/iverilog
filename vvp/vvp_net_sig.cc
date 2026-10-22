/*
 * Copyright (c) 2004-2020 Stephen Williams (steve@icarus.com)
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

# include  "config.h"
# include  "vvp_net.h"
# include  "vvp_net_sig.h"
# include  "statistics.h"
# include  "vpi_priv.h"
# include  <vector>
# include  <cassert>
#నిర్వచితమైతే CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
# include  <map>
#అయితే_ముగింపు

# include  <iostream>

ఉపయోగించి నామాకాశ std;

/*
 * The filter_mask_ method takes as an input the value to propagate,
 * the mask of what is being forced, and returns a propagation
 * mode. In the process, it may update the filtered output value.
 *
 * The input value is the subvector "val" that is placed as "base" in
 * the output. The val may be shorter then the target vector.
 *
 * The "force" vector in the value being force, with the force_mask_
 * member a bit mask of which parts of the force vector really apply.
 */
మూస <తరగతి T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_ముసుగు(స్థిరరాశి T&val, స్థిరరాశి T&force, T&filter, గుర్తులేని పునాది)
{
      ఒకవేళ (!test_force_ముసుగు_is_zero()) {
	      // Some bits are being forced. Go through the
	      // force_mask_ and force value to see which bits are
	      // propagated and which are kept from the forced
	      // value. Update the filter with the filtered result and
	      // return REPL to indicate that some bits have changed,
	      // or STOP if no bits change.
	    తార్కిక propagate_flag = force_propagate_;
	    force_propagate_ = అబద్ధం;
	    ప్రతిజ్ఞా(force_ముసుగు_2.size() == force.size());
	    ప్రతిజ్ఞా((పునాది+val.size()) <= force_ముసుగు_2.size());

	    filter = val;
	    కొరకు (గుర్తులేని idx = 0 ; idx < val.size() ; idx += 1) {
		  ఒకవేళ (force_ముసుగు_2.value(పునాది+idx))
			filter.సమితి_బిట్టు(idx, force.value(పునాది+idx));
		  లేకపోతే
			propagate_flag = నిజం;
	    }

	    ఒకవేళ (propagate_flag) {
		  నిరంతరం_vpi_callbacks();
		  తిరిగి REPL;
	    } లేకపోతే {
		  తిరిగి STOP;
	    }

      } లేకపోతే {
	    నిరంతరం_vpi_callbacks();
	    తిరిగి PROP;
      }
}

మూస <తరగతి T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_ముసుగు(T&val, T force)
{

      ఒకవేళ (test_force_ముసుగు(0)) {
	    val = force;
	    నిరంతరం_vpi_callbacks();
	    తిరిగి REPL;
      }
      నిరంతరం_vpi_callbacks();
      తిరిగి PROP;
}

మూస <తరగతి T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_ఆదానం_ముసుగు(స్థిరరాశి T&val, స్థిరరాశి T&force, T&rep) స్థిరరాశి
{
      ఒకవేళ (test_force_ముసుగు_is_zero())
	    తిరిగి PROP;

      ప్రతిజ్ఞా(force_ముసుగు_2.size() == force.size());

      rep = val;
      కొరకు (గుర్తులేని idx = 0 ; idx < val.size() ; idx += 1) {
	    ఒకవేళ (force_ముసుగు_2.value(idx))
		  rep.సమితి_బిట్టు(idx, force.value(idx));
      }

      తిరిగి REPL;
}

vvp_signal_విలువ::~vvp_signal_విలువ()
{
}

ద్విగుణ vvp_signal_విలువ::real_విలువ() స్థిరరాశి
{
      ప్రతిజ్ఞా(0);
      తిరిగి 0;
}

శూన్య vvp_net_t::force_vec4(స్థిరరాశి vvp_vector4_t&val, స్థిరరాశి vvp_vector2_t&ముసుగు)
{
      ప్రతిజ్ఞా(fil);
      fil->force_fil_vec4(val, ముసుగు);
      fun->force_flag(అబద్ధం);
      vvp_send_vec4(బహిర్గతం_2, val, 0);
}

శూన్య vvp_net_t::force_vec8(స్థిరరాశి vvp_vector8_t&val, స్థిరరాశి vvp_vector2_t&ముసుగు)
{
      ప్రతిజ్ఞా(fil);
      fil->force_fil_vec8(val, ముసుగు);
      fun->force_flag(అబద్ధం);
      vvp_send_vec8(బహిర్గతం_2, val);
}

శూన్య vvp_net_t::force_real(ద్విగుణ val, స్థిరరాశి vvp_vector2_t&ముసుగు)
{
      ప్రతిజ్ఞా(fil);
      fil->force_fil_real(val, ముసుగు);
      fun->force_flag(అబద్ధం);
      vvp_send_real(బహిర్గతం_2, val, 0);
}

/* **** vvp_fun_signal methods **** */

vvp_fun_signal_పునాది::vvp_fun_signal_పునాది()
{
      continuous_కేటాయించు_చురుకైన = అబద్ధం;
      needs_init_ = నిజం;
      cassign_link = 0;
      లెక్క_functors_sig += 1;
}

vvp_fun_signal4_sa::vvp_fun_signal4_sa(గుర్తులేని wid, vvp_bit4_t init)
: bits4_(wid, init)
{
}

/*
 * Nets simply reflect their input to their output.
 *
 * NOTE: It is a quirk of vvp_fun_signal that it has an initial value
 * that needs to be propagated, but after that it only needs to
 * propagate if the value changes. Eliminating duplicate propagations
 * should improve performance, but has the quirk that an input that
 * matches the initial value might not be propagated. The hack used
 * herein is to keep a "needs_init_" flag that is turned false after
 * the first propagation, and forces the first propagation to happen
 * even if it matches the initial value.
 */
శూన్య vvp_fun_signal4_sa::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                   vvp_context_t)
{
      ఎంపిక (ptr.పోర్టు()) {
	  స్థితి 0: // Normal input (feed from net, or set from process)
	      /* If we don't have a continuous assign mask then just
		 copy the bits, otherwise we need to see if there are
		 any holes in the mask so we can set those bits. */
	    ఒకవేళ (కేటాయించు_ముసుగు.size() == 0) {
                  ఒకవేళ (needs_init_ || !bits4_.eeq(బిట్టు)) {
			ప్రతిజ్ఞా(బిట్టు.size() == bits4_.size());
			bits4_ = బిట్టు;
			needs_init_ = అబద్ధం;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    } లేకపోతే {
		  తార్కిక మారింది = అబద్ధం;
		  ప్రతిజ్ఞా(bits4_.size() == కేటాయించు_ముసుగు.size());
		  కొరకు (గుర్తులేని idx = 0 ;  idx < బిట్టు.size() ;  idx += 1) {
			ఒకవేళ (idx >= bits4_.size()) ఆపు;
			ఒకవేళ (కేటాయించు_ముసుగు.value(idx)) కొనసాగు;
			bits4_.సమితి_బిట్టు(idx, బిట్టు.value(idx));
			మారింది = నిజం;
		  }
		  ఒకవేళ (మారింది) {
			needs_init_ = అబద్ధం;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    }
	    ఆపు;

	  స్థితి 1: // Continuous assign value
	      // Handle the simple case of the linked source being wider
	      // than this signal. Note we don't yet support the case of
	      // the linked source being narrower than this signal, or
	      // the case of an expression being assigned.
	    bits4_ = coerce_గమ్యం_వెడల్పు(బిట్టు, bits4_.size());
	    కేటాయించు_ముసుగు = vvp_vector2_t(vvp_vector2_t::FILL1, bits4_.size());
	    ptr.ptr()->send_vec4(bits4_, 0);
	    ఆపు;

	  అప్రమేయ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.పోర్టు());
	    ప్రతిజ్ఞా(0);
	    ఆపు;
      }
}

శూన్య vvp_fun_signal4_sa::recv_vec8(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector8_t&బిట్టు)
{
      recv_vec4(ptr, reduce4(బిట్టు), 0);
}

శూన్య vvp_fun_signal4_sa::recv_vec4_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
				      గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t)
{
      ప్రతిజ్ఞా(bits4_.size() == vwid);
      గుర్తులేని wid = బిట్టు.size();

      ఎంపిక (ptr.పోర్టు()) {
	  స్థితి 0: // Normal input
	    ఒకవేళ (కేటాయించు_ముసుగు.size() == 0) {
                  కొరకు (గుర్తులేని idx = 0 ;  idx < wid ;  idx += 1) {
			ఒకవేళ (పునాది+idx >= bits4_.size()) ఆపు;
			bits4_.సమితి_బిట్టు(పునాది+idx, బిట్టు.value(idx));
		  }
		  needs_init_ = అబద్ధం;
		  ptr.ptr()->send_vec4(bits4_,0);
	    } లేకపోతే {
		  తార్కిక మారింది = అబద్ధం;
		  ప్రతిజ్ఞా(bits4_.size() == కేటాయించు_ముసుగు.size());
		  కొరకు (గుర్తులేని idx = 0 ;  idx < wid ;  idx += 1) {
			ఒకవేళ (పునాది+idx >= bits4_.size()) ఆపు;
			ఒకవేళ (కేటాయించు_ముసుగు.value(పునాది+idx)) కొనసాగు;
			bits4_.సమితి_బిట్టు(పునాది+idx, బిట్టు.value(idx));
			మారింది = నిజం;
		  }
		  ఒకవేళ (మారింది) {
			needs_init_ = అబద్ధం;
			ptr.ptr()->send_vec4(bits4_,0);
		  }
	    }
	    ఆపు;

	  స్థితి 1: // Continuous assign value
	    ఒకవేళ (కేటాయించు_ముసుగు.size() == 0)
		  కేటాయించు_ముసుగు = vvp_vector2_t(vvp_vector2_t::FILL0, bits4_.size());
	    కొరకు (గుర్తులేని idx = 0 ;  idx < wid ;  idx += 1) {
		  ఒకవేళ (పునాది+idx >= bits4_.size())
			ఆపు;
		  bits4_.సమితి_బిట్టు(పునాది+idx, బిట్టు.value(idx));
		  కేటాయించు_ముసుగు.సమితి_బిట్టు(పునాది+idx, 1);
	    }
	    ptr.ptr()->send_vec4(bits4_,0);
	    ఆపు;

	  అప్రమేయ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.పోర్టు());
	    ప్రతిజ్ఞా(0);
	    ఆపు;
      }
}

శూన్య vvp_fun_signal4_sa::recv_vec8_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector8_t&బిట్టు,
				      గుర్తులేని పునాది, గుర్తులేని vwid)
{
      recv_vec4_pv(ptr, reduce4(బిట్టు), పునాది, vwid, 0);
}

శూన్య vvp_fun_signal_పునాది::deassign()
{
      continuous_కేటాయించు_చురుకైన = అబద్ధం;
      కేటాయించు_ముసుగు = vvp_vector2_t();
}

శూన్య vvp_fun_signal_పునాది::deassign_pv(గుర్తులేని పునాది, గుర్తులేని wid)
{
      కొరకు (గుర్తులేని idx = 0 ;  idx < wid ;  idx += 1) {
	    కేటాయించు_ముసుగు.సమితి_బిట్టు(పునాది+idx, 0);
      }

      ఒకవేళ (కేటాయించు_ముసుగు.is_zero()) {
	    కేటాయించు_ముసుగు = vvp_vector2_t();
      }
}

శూన్య automatic_signal_పునాది::release(vvp_net_ptr_t,తార్కిక)
{
      ప్రతిజ్ఞా(0);
}

శూన్య automatic_signal_పునాది::release_pv(vvp_net_ptr_t,గుర్తులేని,గుర్తులేని,తార్కిక)
{
      ప్రతిజ్ఞా(0);
}

గుర్తులేని automatic_signal_పునాది::filter_కొలత() స్థిరరాశి
{
      ప్రతిజ్ఞా(0);
      తిరిగి(0);
}
శూన్య automatic_signal_పునాది::force_fil_vec4(స్థిరరాశి vvp_vector4_t&, స్థిరరాశి vvp_vector2_t&)
{
      ప్రతిజ్ఞా(0);
}
శూన్య automatic_signal_పునాది::force_fil_vec8(స్థిరరాశి vvp_vector8_t&, స్థిరరాశి vvp_vector2_t&)
{
      ప్రతిజ్ఞా(0);
}
శూన్య automatic_signal_పునాది::force_fil_real(ద్విగుణ, స్థిరరాశి vvp_vector2_t&)
{
      ప్రతిజ్ఞా(0);
}
శూన్య automatic_signal_పునాది::get_విలువ(నిర్మాణం t_vpi_విలువ*)
{
      ప్రతిజ్ఞా(0);
}

స్థిరరాశి vvp_vector4_t& vvp_fun_signal4_sa::vec4_unfiltered_విలువ() స్థిరరాశి
{
      తిరిగి bits4_;
}

vvp_fun_signal4_aa::vvp_fun_signal4_aa(గుర్తులేని wid, vvp_bit4_t init)
{
      context_idx_ = vpip_కూడు_అంశం_గమ్యం_context(ఇది, vpip_peek_context_scope());
      కొలత_2 = wid;
      init_ = init;
}

vvp_fun_signal4_aa::~vvp_fun_signal4_aa()
{
      ప్రతిజ్ఞా(0);
}

శూన్య vvp_fun_signal4_aa::alloc_instance(vvp_context_t context)
{
      vvp_సమితి_context_అంశం(context, context_idx_, కొత్త vvp_vector4_t(కొలత_2, init_));
}

శూన్య vvp_fun_signal4_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*బిట్లు = static_cast<vvp_vector4_t*>
            (vvp_get_context_అంశం(context, context_idx_));

      బిట్లు->fill_బిట్లు(init_);
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య vvp_fun_signal4_aa::free_instance(vvp_context_t context)
{
      vvp_vector4_t*బిట్లు = static_cast<vvp_vector4_t*>
            (vvp_get_context_అంశం(context, context_idx_));
      తొలగించు బిట్లు;
}
#అయితే_ముగింపు

/*
 * Continuous and forced assignments are not permitted on automatic
 * variables. So we only expect to receive on port 0.
 */
శూన్య vvp_fun_signal4_aa::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                   vvp_context_t context)
{
      ప్రతిజ్ఞా(ptr.పోర్టు() == 0);
      ప్రతిజ్ఞా(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_అంశం(context, context_idx_));

      ఒకవేళ (!bits4->eeq(బిట్టు)) {
            *bits4 = బిట్టు;
            ptr.ptr()->send_vec4(*bits4, context);
      }
}

శూన్య vvp_fun_signal4_aa::recv_vec4_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
				      గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t context)
{
      ప్రతిజ్ఞా(ptr.పోర్టు() == 0);
      ప్రతిజ్ఞా(కొలత_2 == vwid);
      ప్రతిజ్ఞా(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_అంశం(context, context_idx_));

      గుర్తులేని wid = బిట్టు.size();
      కొరకు (గుర్తులేని idx = 0 ;  idx < wid ;  idx += 1) {
            ఒకవేళ (పునాది+idx >= bits4->size()) ఆపు;
            bits4->సమితి_బిట్టు(పునాది+idx, బిట్టు.value(idx));
      }
      ptr.ptr()->send_vec4(*bits4, context);
}

గుర్తులేని vvp_fun_signal4_aa::విలువ_కొలత() స్థిరరాశి
{
      తిరిగి కొలత_2;
}

vvp_bit4_t vvp_fun_signal4_aa::value(గుర్తులేని idx) స్థిరరాశి
{
      స్థిరరాశి vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_అంశం(context_idx_));

      తిరిగి bits4->value(idx);
}

vvp_scalar_t vvp_fun_signal4_aa::scalar_విలువ(గుర్తులేని idx) స్థిరరాశి
{
      స్థిరరాశి vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_అంశం(context_idx_));

      తిరిగి vvp_scalar_t(bits4->value(idx), 6, 6);
}

శూన్య vvp_fun_signal4_aa::vec4_విలువ(vvp_vector4_t&val) స్థిరరాశి
{
      స్థిరరాశి vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_అంశం(context_idx_));

      val = *bits4;
}

స్థిరరాశి vvp_vector4_t&vvp_fun_signal4_aa::vec4_unfiltered_విలువ() స్థిరరాశి
{
      స్థిరరాశి vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_అంశం(context_idx_));

      తిరిగి *bits4;
}

శూన్య vvp_fun_signal4_aa::క్రియాకారకం తొలగించు(శూన్య*)
{
      ప్రతిజ్ఞా(0);
}

/*
 * Testing for equality, we want a bitwise test instead of an
 * arithmetic test because we want to treat for example -0 different
 * from +0.
 */
తార్కిక బిట్లు_equal(ద్విగుణ a, ద్విగుణ b)
{
      తిరిగి memcmp(&a, &b, పరిమాణం a) == 0;
}

vvp_fun_signal_real_sa::vvp_fun_signal_real_sa()
{
      బిట్లు_2 = 0.0;
}

ద్విగుణ vvp_fun_signal_real_sa::real_unfiltered_విలువ() స్థిరరాశి
{
      తిరిగి బిట్లు_2;
}

శూన్య vvp_fun_signal_real_sa::recv_real(vvp_net_ptr_t ptr, ద్విగుణ బిట్టు,
                                       vvp_context_t)
{
      ఎంపిక (ptr.పోర్టు()) {
	  స్థితి 0:
	    ఒకవేళ (!continuous_కేటాయించు_చురుకైన) {
                  ఒకవేళ (needs_init_ || !బిట్లు_equal(బిట్లు_2, బిట్టు)) {
			బిట్లు_2 = బిట్టు;
			needs_init_ = అబద్ధం;
			ptr.ptr()->send_real(బిట్టు, 0);
		  }
	    }
	    ఆపు;

	  స్థితి 1: // Continuous assign value
	    continuous_కేటాయించు_చురుకైన = నిజం;
	    బిట్లు_2 = బిట్టు;
	    ptr.ptr()->send_real(బిట్టు, 0);
	    ఆపు;

	  అప్రమేయ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.పోర్టు());
	    ప్రతిజ్ఞా(0);
	    ఆపు;
      }
}

vvp_fun_signal_real_aa::vvp_fun_signal_real_aa()
{
      context_idx_ = vpip_కూడు_అంశం_గమ్యం_context(ఇది, vpip_peek_context_scope());
}

vvp_fun_signal_real_aa::~vvp_fun_signal_real_aa()
{
      ప్రతిజ్ఞా(0);
}

శూన్య vvp_fun_signal_real_aa::alloc_instance(vvp_context_t context)
{
      ద్విగుణ*బిట్లు = కొత్త ద్విగుణ;
      vvp_సమితి_context_అంశం(context, context_idx_, బిట్లు);

      *బిట్లు = 0.0;
}

శూన్య vvp_fun_signal_real_aa::reset_instance(vvp_context_t context)
{
      ద్విగుణ*బిట్లు = static_cast<ద్విగుణ*>
            (vvp_get_context_అంశం(context, context_idx_));

      *బిట్లు = 0.0;
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య vvp_fun_signal_real_aa::free_instance(vvp_context_t context)
{
      ద్విగుణ*బిట్లు = static_cast<ద్విగుణ*>
            (vvp_get_context_అంశం(context, context_idx_));
      తొలగించు బిట్లు;
}
#అయితే_ముగింపు

ద్విగుణ vvp_fun_signal_real_aa::real_unfiltered_విలువ() స్థిరరాశి
{
      స్థిరరాశి ద్విగుణ*బిట్లు = static_cast<ద్విగుణ*>
            (vthread_get_rd_context_అంశం(context_idx_));

      తిరిగి *బిట్లు;
}

ద్విగుణ vvp_fun_signal_real_aa::real_విలువ() స్థిరరాశి
{
      తిరిగి real_unfiltered_విలువ();
}

శూన్య vvp_fun_signal_real_aa::recv_real(vvp_net_ptr_t ptr, ద్విగుణ బిట్టు,
                                       vvp_context_t context)
{
      ప్రతిజ్ఞా(ptr.పోర్టు() == 0);
      ప్రతిజ్ఞా(context);

      ద్విగుణ*బిట్లు = static_cast<ద్విగుణ*>
            (vvp_get_context_అంశం(context, context_idx_));

      ఒకవేళ (!బిట్లు_equal(*బిట్లు,బిట్టు)) {
            *బిట్లు = బిట్టు;
            ptr.ptr()->send_real(బిట్టు, context);
      }
}

గుర్తులేని vvp_fun_signal_real_aa::విలువ_కొలత() స్థిరరాశి
{
      ప్రతిజ్ఞా(0);
      తిరిగి 1;
}

vvp_bit4_t vvp_fun_signal_real_aa::value(గుర్తులేని) స్థిరరాశి
{
      ప్రతిజ్ఞా(0);
      తిరిగి BIT4_X;
}

vvp_scalar_t vvp_fun_signal_real_aa::scalar_విలువ(గుర్తులేని) స్థిరరాశి
{
      ప్రతిజ్ఞా(0);
      తిరిగి vvp_scalar_t();
}

శూన్య vvp_fun_signal_real_aa::vec4_విలువ(vvp_vector4_t&) స్థిరరాశి
{
      ప్రతిజ్ఞా(0);
}

శూన్య* vvp_fun_signal_real_aa::క్రియాకారకం కొత్త(std::size_t size)
{
      తిరిగి vvp_net_fun_t::కుప్ప.alloc(size);
}

శూన్య vvp_fun_signal_real_aa::క్రియాకారకం తొలగించు(శూన్య*)
{
      ప్రతిజ్ఞా(0);
}


vvp_fun_signal_string_sa::vvp_fun_signal_string_sa()
{
}

శూన్య vvp_fun_signal_string_sa::recv_string(vvp_net_ptr_t ptr, స్థిరరాశి std::string&బిట్టు,
					   vvp_context_t)
{
      ప్రతిజ్ఞా(ptr.పోర్టు() == 0);

      ఒకవేళ (needs_init_ || విలువ_2 != బిట్టు) {
	    విలువ_2 = బిట్టు;
	    needs_init_ = అబద్ధం;

	    ptr.ptr()->send_string(బిట్టు, 0);
      }
}

స్థిరరాశి string& vvp_fun_signal_string_sa::get_string() స్థిరరాశి
{
      తిరిగి విలువ_2;
}

vvp_fun_signal_string_aa::vvp_fun_signal_string_aa()
{
      context_idx_ = vpip_కూడు_అంశం_గమ్యం_context(ఇది, vpip_peek_context_scope());
}

vvp_fun_signal_string_aa::~vvp_fun_signal_string_aa()
{
      ప్రతిజ్ఞా(0);
}

శూన్య vvp_fun_signal_string_aa::alloc_instance(vvp_context_t context)
{
      string*బిట్లు = కొత్త std::string;
      vvp_సమితి_context_అంశం(context, context_idx_, బిట్లు);
      *బిట్లు = "";
}

శూన్య vvp_fun_signal_string_aa::reset_instance(vvp_context_t context)
{
      string*బిట్లు = static_cast<std::string*>
	    (vvp_get_context_అంశం(context, context_idx_));
      *బిట్లు = "";
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య vvp_fun_signal_string_aa::free_instance(vvp_context_t context)
{
      string*బిట్లు = static_cast<std::string*>
            (vvp_get_context_అంశం(context, context_idx_));
      తొలగించు బిట్లు;
}
#అయితే_ముగింపు

శూన్య vvp_fun_signal_string_aa::recv_string(vvp_net_ptr_t ptr, స్థిరరాశి std::string&బిట్టు, vvp_context_t context)
{
      ప్రతిజ్ఞా(ptr.పోర్టు() == 0);
      ప్రతిజ్ఞా(context);

      string*బిట్లు = static_cast<std::string*>
	    (vvp_get_context_అంశం(context, context_idx_));

      ఒకవేళ (*బిట్లు != బిట్టు) {
	    *బిట్లు = బిట్టు;
	    ptr.ptr()->send_string(బిట్టు, context);
      }
}

గుర్తులేని vvp_fun_signal_string_aa::విలువ_కొలత() స్థిరరాశి
{
      ప్రతిజ్ఞా(0);
      తిరిగి 1;
}

vvp_bit4_t vvp_fun_signal_string_aa::value(గుర్తులేని) స్థిరరాశి
{
      ప్రతిజ్ఞా(0);
      తిరిగి BIT4_X;
}

vvp_scalar_t vvp_fun_signal_string_aa::scalar_విలువ(గుర్తులేని) స్థిరరాశి
{
      ప్రతిజ్ఞా(0);
      తిరిగి vvp_scalar_t();
}

శూన్య vvp_fun_signal_string_aa::vec4_విలువ(vvp_vector4_t&) స్థిరరాశి
{
      ప్రతిజ్ఞా(0);
}

ద్విగుణ vvp_fun_signal_string_aa::real_విలువ() స్థిరరాశి
{
      ప్రతిజ్ఞా(0);
      తిరిగి 0.0;
}

స్థిరరాశి std::string& vvp_fun_signal_string_aa::get_string() స్థిరరాశి
{
      స్థిరరాశి string*బిట్లు = static_cast<std::string*>
            (vthread_get_rd_context_అంశం(context_idx_));

      తిరిగి *బిట్లు;
}

శూన్య* vvp_fun_signal_string_aa::క్రియాకారకం కొత్త(std::size_t size)
{
      తిరిగి vvp_net_fun_t::కుప్ప.alloc(size);
}

శూన్య vvp_fun_signal_string_aa::క్రియాకారకం తొలగించు(శూన్య*)
{
      ప్రతిజ్ఞా(0);
}

  /* OBJECT signals */

vvp_fun_signal_object_sa::vvp_fun_signal_object_sa(గుర్తులేని size)
: vvp_fun_signal_object(size)
{
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య vvp_fun_signal_object_aa::free_instance(vvp_context_t context)
{
      vvp_object_t*బిట్లు = static_cast<vvp_object_t*>
            (vvp_get_context_అంశం(context, context_idx_));
      తొలగించు బిట్లు;
}
#అయితే_ముగింపు

శూన్య vvp_fun_signal_object_sa::recv_object(vvp_net_ptr_t ptr, vvp_object_t బిట్టు,
					   vvp_context_t)
{
      ప్రతిజ్ఞా(ptr.పోర్టు() == 0);

      ఒకవేళ (needs_init_ || విలువ_2 != బిట్టు) {
	    విలువ_2 = బిట్టు;
	    needs_init_ = అబద్ధం;

	    ptr.ptr()->send_object(బిట్టు, 0);
      }
}

vvp_object_t vvp_fun_signal_object_sa::get_object() స్థిరరాశి
{
      తిరిగి విలువ_2;
}

vvp_fun_signal_object_aa::vvp_fun_signal_object_aa(గుర్తులేని size)
: vvp_fun_signal_object(size)
{
      context_idx_ = vpip_కూడు_అంశం_గమ్యం_context(ఇది, vpip_peek_context_scope());
}

vvp_fun_signal_object_aa::~vvp_fun_signal_object_aa()
{
      ప్రతిజ్ఞా(0);
}

శూన్య vvp_fun_signal_object_aa::alloc_instance(vvp_context_t context)
{
      vvp_object_t*బిట్లు = కొత్త vvp_object_t;
      vvp_సమితి_context_అంశం(context, context_idx_, బిట్లు);
      బిట్లు->reset();
}

శూన్య vvp_fun_signal_object_aa::reset_instance(vvp_context_t context)
{
      vvp_object_t*బిట్లు = static_cast<vvp_object_t*>
	    (vvp_get_context_అంశం(context, context_idx_));
      బిట్లు->reset();
}

vvp_object_t vvp_fun_signal_object_aa::get_object() స్థిరరాశి
{
      స్థిరరాశి vvp_object_t*బిట్లు = static_cast<vvp_object_t*>
	    (vthread_get_rd_context_అంశం(context_idx_));
      తిరిగి *బిట్లు;
}

శూన్య vvp_fun_signal_object_aa::recv_object(vvp_net_ptr_t ptr, vvp_object_t బిట్టు,
					   vvp_context_t context)
{
      ప్రతిజ్ఞా(ptr.పోర్టు() == 0);
      ప్రతిజ్ఞా(context);

      vvp_object_t*బిట్లు = static_cast<vvp_object_t*>
	    (vvp_get_context_అంశం(context, context_idx_));

      ఒకవేళ (*బిట్లు != బిట్టు) {
	    *బిట్లు = బిట్టు;
	    ptr.ptr()->send_object(బిట్టు, context);
      }
}

గుర్తులేని vvp_fun_signal_object_aa::విలువ_కొలత() స్థిరరాశి
{
      తిరిగి 1;
}

vvp_bit4_t vvp_fun_signal_object_aa::value(గుర్తులేని) స్థిరరాశి
{
      ప్రతిజ్ఞా(0);
      తిరిగి BIT4_X;
}

vvp_scalar_t vvp_fun_signal_object_aa::scalar_విలువ(గుర్తులేని) స్థిరరాశి
{
      ప్రతిజ్ఞా(0);
      తిరిగి vvp_scalar_t();
}

శూన్య vvp_fun_signal_object_aa::vec4_విలువ(vvp_vector4_t&) స్థిరరాశి
{
      ప్రతిజ్ఞా(0);
}

శూన్య* vvp_fun_signal_object_aa::క్రియాకారకం కొత్త(std::size_t size)
{
      తిరిగి vvp_net_fun_t::కుప్ప.alloc(size);
}

శూన్య vvp_fun_signal_object_aa::క్రియాకారకం తొలగించు(శూన్య*)
{
      ప్రతిజ్ఞా(0);
}

  /* **** */

vvp_fun_force::vvp_fun_force()
{
}

vvp_fun_force::~vvp_fun_force()
{
}

శూన్య vvp_fun_force::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
			      vvp_context_t)
{
      ప్రతిజ్ఞా(ptr.పోర్టు() == 0);
      vvp_net_t*net = ptr.ptr();

      vvp_net_t*dst = net->పోర్టు[3].ptr();
      ప్రతిజ్ఞా(dst->fil);

      dst->force_vec4(coerce_గమ్యం_వెడల్పు(బిట్టు, dst->fil->filter_కొలత()), vvp_vector2_t(vvp_vector2_t::FILL1, dst->fil->filter_కొలత()));
}

శూన్య vvp_fun_force::recv_real(vvp_net_ptr_t ptr, ద్విగుణ బిట్టు, vvp_context_t)
{
      ప్రతిజ్ఞా(ptr.పోర్టు() == 0);
      vvp_net_t*net = ptr.ptr();
      vvp_net_t*dst = net->పోర్టు[3].ptr();
      dst->force_real(బిట్టు, vvp_vector2_t(vvp_vector2_t::FILL1, 1));
}

vvp_wire_పునాది::vvp_wire_పునాది()
{
}

vvp_wire_పునాది::~vvp_wire_పునాది()
{
}

vvp_bit4_t vvp_wire_పునాది::driven_విలువ(గుర్తులేని) స్థిరరాశి
{
      ప్రతిజ్ఞా(0);
      తిరిగి BIT4_X;
}

తార్కిక vvp_wire_పునాది::is_forced(గుర్తులేని) స్థిరరాశి
{
      ప్రతిజ్ఞా(0);
      తిరిగి అబద్ధం;
}

vvp_wire_vec4::vvp_wire_vec4(గుర్తులేని wid, vvp_bit4_t init)
: bits4_(wid, init)
{
      needs_init_ = నిజం;
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec4(స్థిరరాశి vvp_vector4_t&బిట్టు, vvp_vector4_t&rep,
						 గుర్తులేని పునాది, గుర్తులేని vwid)
{
	// Special case! the input bit is 0 wid. Interpret this as a
	// vector of BIT4_X to match the width of the bits4_ vector.
	// FIXME! This is a hack to work around some buggy gate
	// implementations! This should be removed!
      ఒకవేళ (పునాది==0 && vwid==0) {
	    vvp_vector4_t tmp (bits4_.size(), BIT4_X);
	    ఒకవేళ (bits4_ .eeq(tmp) && !needs_init_) తిరిగి STOP;
	    bits4_ = tmp;
	    needs_init_ = అబద్ధం;
	    తిరిగి filter_ముసుగు(tmp, force4_, rep, 0);
      }

      ఒకవేళ (vwid != bits4_.size()) {
	    cerr << "Internal error: Input vector expected width="
		 << bits4_.size() << ", got "
		 << "bit=" << బిట్టు << ", base=" << పునాది << ", vwid=" << vwid
		 << పంక్తి_ముగింపు;
      }
      ప్రతిజ్ఞా(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      ఒకవేళ (పునాది==0 && బిట్టు.size()==vwid) {
	    ఒకవేళ (bits4_ .eeq( బిట్టు ) && !needs_init_) తిరిగి STOP;
	    bits4_ = బిట్టు;
      } లేకపోతే {
	    తార్కిక rc = bits4_.సమితి_vec(పునాది, బిట్టు);
	    ఒకవేళ (rc == అబద్ధం && !needs_init_) తిరిగి STOP;
      }

      needs_init_ = అబద్ధం;
      తిరిగి filter_ముసుగు(బిట్టు, force4_, rep, పునాది);
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec8(స్థిరరాశి vvp_vector8_t&బిట్టు,
                                                 vvp_vector8_t&rep,
                                                 గుర్తులేని పునాది,
                                                 గుర్తులేని vwid)
{
      ప్రతిజ్ఞా(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      vvp_vector4_t bit4 (reduce4(బిట్టు));
      ఒకవేళ (పునాది==0 && bit4.size()==vwid) {
	    ఒకవేళ (bits4_ .eeq( bit4 ) && !needs_init_) తిరిగి STOP;
	    bits4_ = bit4;
      } లేకపోతే {
	    తార్కిక rc = bits4_.సమితి_vec(పునాది, bit4);
	    ఒకవేళ (rc == అబద్ధం && !needs_init_) తిరిగి STOP;
      }

      needs_init_ = అబద్ధం;
      తిరిగి filter_ముసుగు(బిట్టు, vvp_vector8_t(force4_,6,6), rep, పునాది);
}

గుర్తులేని vvp_wire_vec4::filter_కొలత() స్థిరరాశి
{
      తిరిగి bits4_.size();
}

శూన్య vvp_wire_vec4::force_fil_vec4(స్థిరరాశి vvp_vector4_t&val, స్థిరరాశి vvp_vector2_t&ముసుగు)
{
      force_ముసుగు(ముసుగు);

      ఒకవేళ (force4_.size() == 0) {
	    force4_ = val;
      } లేకపోతే {
	    కొరకు (గుర్తులేని idx = 0; idx < ముసుగు.size() ; idx += 1) {
		  ఒకవేళ (ముసుగు.value(idx) == 0)
			కొనసాగు;

		  force4_.సమితి_బిట్టు(idx, val.value(idx));
	    }
      }
      నిరంతరం_vpi_callbacks();
}

శూన్య vvp_wire_vec4::force_fil_vec8(స్థిరరాశి vvp_vector8_t&, స్థిరరాశి vvp_vector2_t&)
{
      ప్రతిజ్ఞా(0);
}

శూన్య vvp_wire_vec4::force_fil_real(ద్విగుణ, స్థిరరాశి vvp_vector2_t&)
{
      ప్రతిజ్ఞా(0);
}

శూన్య vvp_wire_vec4::release(vvp_net_ptr_t ptr, తార్కిక net_flag)
{
      vvp_vector2_t ముసుగు (vvp_vector2_t::FILL1, bits4_.size());
      ఒకవేళ (net_flag) {
	      // Wires revert to their unforced value after release.
            release_ముసుగు(ముసుగు);
	    needs_init_ = ! force4_ .eeq(bits4_);
	    ptr.ptr()->send_vec4(bits4_, 0);
	    నిరంతరం_vpi_callbacks();
      } లేకపోతే {
	      // Variables keep the current value.
	    vvp_vector4_t res (bits4_.size());
	    కొరకు (గుర్తులేని idx=0; idx<bits4_.size(); idx += 1)
		  res.సమితి_బిట్టు(idx,value(idx));
            release_ముసుగు(ముసుగు);
	    ptr.ptr()->fun->recv_vec4(ptr, res, 0);
      }
}

శూన్య vvp_wire_vec4::release_pv(vvp_net_ptr_t ptr, గుర్తులేని పునాది, గుర్తులేని wid, తార్కిక net_flag)
{
      ప్రతిజ్ఞా(bits4_.size() >= పునాది + wid);

      vvp_vector2_t ముసుగు (vvp_vector2_t::FILL0, bits4_.size());
      కొరకు (గుర్తులేని idx = 0 ; idx < wid ; idx += 1)
	    ముసుగు.సమితి_బిట్టు(పునాది+idx, 1);

      ఒకవేళ (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_ముసుగు(ముసుగు);
	    needs_init_ = ! force4_.subvalue(పునాది,wid) .eeq(bits4_.subvalue(పునాది,wid));
	    ptr.ptr()->send_vec4_pv(bits4_.subvalue(పునాది,wid),
				    పునాది, bits4_.size(), 0);
	    నిరంతరం_vpi_callbacks();
      } లేకపోతే {
	      // Variables keep the current value.
	    vvp_vector4_t res (wid);
	    కొరకు (గుర్తులేని idx=0; idx<wid; idx += 1)
		  res.సమితి_బిట్టు(idx,value(పునాది+idx));
	    release_ముసుగు(ముసుగు);
	    ptr.ptr()->fun->recv_vec4_pv(ptr, res, పునాది, bits4_.size(), 0);
      }
}

గుర్తులేని vvp_wire_vec4::విలువ_కొలత() స్థిరరాశి
{
      తిరిగి bits4_.size();
}

vvp_bit4_t vvp_wire_vec4::filtered_విలువ(గుర్తులేని idx) స్థిరరాశి
{
      ఒకవేళ (test_force_ముసుగు(idx))
	    తిరిగి force4_.value(idx);
      లేకపోతే
	    తిరిగి bits4_.value(idx);
}

vvp_bit4_t vvp_wire_vec4::value(గుర్తులేని idx) స్థిరరాశి
{
      తిరిగి filtered_విలువ(idx);
}

vvp_scalar_t vvp_wire_vec4::scalar_విలువ(గుర్తులేని idx) స్థిరరాశి
{
      తిరిగి vvp_scalar_t(value(idx),6,6);
}

శూన్య vvp_wire_vec4::vec4_విలువ(vvp_vector4_t&val) స్థిరరాశి
{
      val = bits4_;
      ఒకవేళ (test_force_ముసుగు_is_zero())
	    తిరిగి;

      కొరకు (గుర్తులేని idx = 0 ; idx < bits4_.size() ; idx += 1)
	    val.సమితి_బిట్టు(idx, filtered_విలువ(idx));
}

vvp_bit4_t vvp_wire_vec4::driven_విలువ(గుర్తులేని idx) స్థిరరాశి
{
      తిరిగి bits4_.value(idx);
}

తార్కిక vvp_wire_vec4::is_forced(గుర్తులేని idx) స్థిరరాశి
{
      తిరిగి test_force_ముసుగు(idx);
}

vvp_wire_vec8::vvp_wire_vec8(గుర్తులేని wid)
: bits8_(wid)
{
      needs_init_ = నిజం;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec4(స్థిరరాశి vvp_vector4_t&బిట్టు,
                                                 vvp_vector4_t&rep,
                                                 గుర్తులేని పునాది,
                                                 గుర్తులేని vwid)
{
	// For now there is no support for a non-zero base.
      ప్రతిజ్ఞా(0 == పునాది);
      ప్రతిజ్ఞా(bits8_.size() == vwid);
      ప్రతిజ్ఞా(bits8_.size() == బిట్టు.size());
	// QUESTION: Is it really correct to propagate a vec4 if this
	// is a vec8 node? In fact, it is really possible for a vec4
	// value to get through to a vec8 filter?
      vvp_vector8_t rep8;
      prop_t rc = filter_vec8(vvp_vector8_t(బిట్టు,6,6), rep8, 0, vwid);
      ఒకవేళ (rc == REPL)
	    rep = reduce4(rep8);

      needs_init_ = అబద్ధం;
      తిరిగి rc;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec8(స్థిరరాశి vvp_vector8_t&బిట్టు, vvp_vector8_t&rep, గుర్తులేని పునాది, గుర్తులేని vwid)
{
      ప్రతిజ్ఞా(vwid == bits8_.size());
	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      ఒకవేళ (పునాది==0 && బిట్టు.size()==vwid) {
	    ఒకవేళ (bits8_.eeq(బిట్టు) && !needs_init_) తిరిగి STOP;
	    bits8_ = బిట్టు;
      } లేకపోతే {
	    ఒకవేళ (bits8_.subvalue(పునాది, బిట్టు.size()).eeq(బిట్టు) &&
		!needs_init_) {
		  తిరిగి STOP;
	    }
	    ఒకవేళ (bits8_.size() == 0)
		  bits8_ = vvp_vector8_t(vwid);
	    ప్రతిజ్ఞా(bits8_.size() == vwid);
	    bits8_.సమితి_vec(పునాది, బిట్టు);
      }
      needs_init_ = అబద్ధం;
      తిరిగి filter_ముసుగు(బిట్టు, force8_, rep, పునాది);
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_ఆదానం_vec8(స్థిరరాశి vvp_vector8_t&బిట్టు, vvp_vector8_t&rep) స్థిరరాశి
{
      తిరిగి filter_ఆదానం_ముసుగు(బిట్టు, force8_, rep);
}

గుర్తులేని vvp_wire_vec8::filter_కొలత() స్థిరరాశి
{
      తిరిగి bits8_.size();
}

శూన్య vvp_wire_vec8::force_fil_vec4(స్థిరరాశి vvp_vector4_t&val, స్థిరరాశి vvp_vector2_t&ముసుగు)
{
      force_fil_vec8(vvp_vector8_t(val,6,6), ముసుగు);
}

శూన్య vvp_wire_vec8::force_fil_vec8(స్థిరరాశి vvp_vector8_t&val, స్థిరరాశి vvp_vector2_t&ముసుగు)
{
      force_ముసుగు(ముసుగు);

      ఒకవేళ (force8_.size() == 0) {
	    force8_ = val;
      } లేకపోతే {
	    కొరకు (గుర్తులేని idx = 0; idx < ముసుగు.size() ; idx += 1) {
		  ఒకవేళ (ముసుగు.value(idx) == 0)
			కొనసాగు;

		  force8_.సమితి_బిట్టు(idx, val.value(idx));
	    }
      }
      నిరంతరం_vpi_callbacks();
}

శూన్య vvp_wire_vec8::force_fil_real(ద్విగుణ, స్థిరరాశి vvp_vector2_t&)
{
      ప్రతిజ్ఞా(0);
}

శూన్య vvp_wire_vec8::release(vvp_net_ptr_t ptr, తార్కిక net_flag)
{
	// Wires revert to their unforced value after release.
      vvp_vector2_t ముసుగు (vvp_vector2_t::FILL1, bits8_.size());
      release_ముసుగు(ముసుగు);
      ఒకవేళ (net_flag) {
	    needs_init_ = !force8_ .eeq(bits8_);
	    ptr.ptr()->send_vec8(bits8_);
      } లేకపోతే {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    ప్రతిజ్ఞా(0);
//	    ptr.ptr()->fun->recv_vec8(ptr, force8_);
      }
}

శూన్య vvp_wire_vec8::release_pv(vvp_net_ptr_t ptr, గుర్తులేని పునాది, గుర్తులేని wid, తార్కిక net_flag)
{
      ప్రతిజ్ఞా(bits8_.size() >= పునాది + wid);

      vvp_vector2_t ముసుగు (vvp_vector2_t::FILL0, bits8_.size());
      కొరకు (గుర్తులేని idx = 0 ; idx < wid ; idx += 1)
	    ముసుగు.సమితి_బిట్టు(పునాది+idx, 1);

      release_ముసుగు(ముసుగు);

      ఒకవేళ (net_flag) {
	    needs_init_ = !force8_.subvalue(పునాది,wid) .eeq((bits8_.subvalue(పునాది,wid)));
	    ptr.ptr()->send_vec8_pv(bits8_.subvalue(పునాది,wid),
				    పునాది, bits8_.size());
	    నిరంతరం_vpi_callbacks();
      } లేకపోతే {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    ప్రతిజ్ఞా(0);
//	    ptr.ptr()->fun->recv_vec8_pv(ptr, force8_.subvalue(base,wid),
//					 base, force8_.size());
      }
}

గుర్తులేని vvp_wire_vec8::విలువ_కొలత() స్థిరరాశి
{
      తిరిగి bits8_.size();
}

vvp_scalar_t vvp_wire_vec8::filtered_విలువ(గుర్తులేని idx) స్థిరరాశి
{
      ఒకవేళ (test_force_ముసుగు(idx))
	    తిరిగి force8_.value(idx);
      లేకపోతే
	    తిరిగి bits8_.value(idx);
}

vvp_bit4_t vvp_wire_vec8::value(గుర్తులేని idx) స్థిరరాశి
{
      తిరిగి filtered_విలువ(idx).value();
}

vvp_scalar_t vvp_wire_vec8::scalar_విలువ(గుర్తులేని idx) స్థిరరాశి
{
      తిరిగి filtered_విలువ(idx);
}

vvp_vector8_t vvp_wire_vec8::vec8_విలువ() స్థిరరాశి
{
      vvp_vector8_t tmp = bits8_;
      కొరకు (గుర్తులేని idx = 0 ; idx < bits8_.size() ; idx += 1)
	    tmp.సమితి_బిట్టు(idx, filtered_విలువ(idx));
      తిరిగి tmp;
}

శూన్య vvp_wire_vec8::vec4_విలువ(vvp_vector4_t&val) స్థిరరాశి
{
      val = reduce4(vec8_విలువ());
}

vvp_bit4_t vvp_wire_vec8::driven_విలువ(గుర్తులేని idx) స్థిరరాశి
{
      తిరిగి bits8_.value(idx).value();
}

తార్కిక vvp_wire_vec8::is_forced(గుర్తులేని idx) స్థిరరాశి
{
      తిరిగి test_force_ముసుగు(idx);
}

vvp_wire_real::vvp_wire_real()
: బిట్టు_2(0.0), force_(0.0)
{
}

vvp_net_fil_t::prop_t vvp_wire_real::filter_real(ద్విగుణ&బిట్టు)
{
      ఒకవేళ (బిట్లు_equal(బిట్టు_2, బిట్టు) && !needs_init_) తిరిగి STOP;
      బిట్టు_2 = బిట్టు;
      needs_init_ = అబద్ధం;
      తిరిగి filter_ముసుగు(బిట్టు, force_);
}

గుర్తులేని vvp_wire_real::filter_కొలత() స్థిరరాశి
{
      ప్రతిజ్ఞా(0);
      తిరిగి 0;
}

శూన్య vvp_wire_real::force_fil_vec4(స్థిరరాశి vvp_vector4_t&, స్థిరరాశి vvp_vector2_t&)
{
      ప్రతిజ్ఞా(0);
}

శూన్య vvp_wire_real::force_fil_vec8(స్థిరరాశి vvp_vector8_t&, స్థిరరాశి vvp_vector2_t&)
{
      ప్రతిజ్ఞా(0);
}

శూన్య vvp_wire_real::force_fil_real(ద్విగుణ val, స్థిరరాశి vvp_vector2_t&ముసుగు)
{
      force_ముసుగు(ముసుగు);
      ఒకవేళ (ముసుగు.value(0))
	    force_ = val;

      నిరంతరం_vpi_callbacks();
}

శూన్య vvp_wire_real::release(vvp_net_ptr_t ptr, తార్కిక net_flag)
{
      vvp_vector2_t ముసుగు (vvp_vector2_t::FILL1, 1);
      ఒకవేళ (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_ముసుగు(ముసుగు);
	    needs_init_ = !బిట్లు_equal(force_, బిట్టు_2);
	    ptr.ptr()->send_real(బిట్టు_2, 0);
      } లేకపోతే {
	      // Variables keep the current value.
	    ద్విగుణ res =  real_విలువ();
	    release_ముసుగు(ముసుగు);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
}

శూన్య vvp_wire_real::release_pv(vvp_net_ptr_t, గుర్తులేని, గుర్తులేని, తార్కిక)
{
      ప్రతిజ్ఞా(0);
#ఒకవేళ 0
	// A real is a single value. If for some reason this part release
	// can happen the following code should work correctly (requires
	// a base of 0 and a width of 1).
      vvp_vector2_t ముసుగు (vvp_vector2_t::FILL1, 1);
      ప్రతిజ్ఞా(పునాది == 0 && wid == 1);

      ఒకవేళ (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_ముసుగు(ముసుగు);
	    ptr.ptr()->send_real(బిట్టు_2, 0);
      } లేకపోతే {
	      // Variables keep the current value.
	    ద్విగుణ res =  real_విలువ();
	    release_ముసుగు(ముసుగు);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
#అయితే_ముగింపు
}

గుర్తులేని vvp_wire_real::విలువ_కొలత() స్థిరరాశి
{
      ప్రతిజ్ఞా(0);
      తిరిగి 1;
}

vvp_bit4_t vvp_wire_real::value(గుర్తులేని) స్థిరరాశి
{
      ప్రతిజ్ఞా(0);
      తిరిగి BIT4_X;
}

vvp_scalar_t vvp_wire_real::scalar_విలువ(గుర్తులేని) స్థిరరాశి
{
      ప్రతిజ్ఞా(0);
      తిరిగి vvp_scalar_t();
}

శూన్య vvp_wire_real::vec4_విలువ(vvp_vector4_t&) స్థిరరాశి
{
      ప్రతిజ్ఞా(0);
}

ద్విగుణ vvp_wire_real::real_విలువ() స్థిరరాశి
{
      ఒకవేళ (test_force_ముసుగు(0))
	    తిరిగి force_;
      లేకపోతే
	    తిరిగి బిట్టు_2;
}

#ఒకవేళ 0
vvp_wire_string::vvp_wire_string()
{
}

గుర్తులేని vvp_wire_string::filter_కొలత() స్థిరరాశి
{
      ప్రతిజ్ఞా(0);
      తిరిగి 0;
}

శూన్య vvp_wire_string::force_fil_vec4(స్థిరరాశి vvp_vector4_t&, స్థిరరాశి vvp_vector2_t&)
{
      ప్రతిజ్ఞా(0);
}
శూన్య vvp_wire_string::force_fil_vec8(స్థిరరాశి vvp_vector8_t&, స్థిరరాశి vvp_vector2_t&)
{
      ప్రతిజ్ఞా(0);
}
శూన్య vvp_wire_string::force_fil_real(ద్విగుణ, స్థిరరాశి vvp_vector2_t&)
{
      ప్రతిజ్ఞా(0);
}

శూన్య vvp_wire_string::release(vvp_net_ptr_t ptr, తార్కిక net_flag)
{
      ప్రతిజ్ఞా(0);
}

శూన్య vvp_wire_string::release_pv(vvp_net_ptr_t, గుర్తులేని, గుర్తులేని, తార్కిక)
{
      ప్రతిజ్ఞా(0);
}

గుర్తులేని vvp_wire_string::విలువ_కొలత() స్థిరరాశి
{
      ప్రతిజ్ఞా(0);
      తిరిగి 1;
}

vvp_bit4_t vvp_wire_string::value(గుర్తులేని) స్థిరరాశి
{
      ప్రతిజ్ఞా(0);
      తిరిగి BIT4_X;
}

vvp_scalar_t vvp_wire_string::scalar_విలువ(గుర్తులేని) స్థిరరాశి
{
      ప్రతిజ్ఞా(0);
      తిరిగి vvp_scalar_t();
}

శూన్య vvp_wire_string::vec4_విలువ(vvp_vector4_t&) స్థిరరాశి
{
      ప్రతిజ్ఞా(0);
}

ద్విగుణ vvp_wire_string::real_విలువ() స్థిరరాశి
{
      ప్రతిజ్ఞా(0);
      తిరిగి 0.0;
}
#అయితే_ముగింపు
