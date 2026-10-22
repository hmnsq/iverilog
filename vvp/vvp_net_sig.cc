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
#که_تعریف_شوی CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
# include  <map>
#د_که_پای

# include  <iostream>

کارول نطاق std;

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
کينډۍ <صنف T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_ماسک(ثابت T&val, ثابت T&force, T&filter, بینښه بنسټ)
{
      إذا (!test_force_ماسک_is_zero()) {
	      // Some bits are being forced. Go through the
	      // force_mask_ and force value to see which bits are
	      // propagated and which are kept from the forced
	      // value. Update the filter with the filtered result and
	      // return REPL to indicate that some bits have changed,
	      // or STOP if no bits change.
	    منطقي propagate_flag = force_propagate_;
	    force_propagate_ = خطأ;
	    أكد(force_ماسک_2.size() == force.size());
	    أكد((بنسټ+val.size()) <= force_ماسک_2.size());

	    filter = val;
	    لكل (بینښه idx = 0 ; idx < val.size() ; idx += 1) {
		  إذا (force_ماسک_2.value(بنسټ+idx))
			filter.سېټ_بټ(idx, force.value(بنسټ+idx));
		  وإلا
			propagate_flag = ریښتیا;
	    }

	    إذا (propagate_flag) {
		  پرلپسې_vpi_callbacks();
		  أرجع REPL;
	    } وإلا {
		  أرجع STOP;
	    }

      } وإلا {
	    پرلپسې_vpi_callbacks();
	    أرجع PROP;
      }
}

کينډۍ <صنف T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_ماسک(T&val, T force)
{

      إذا (test_force_ماسک(0)) {
	    val = force;
	    پرلپسې_vpi_callbacks();
	    أرجع REPL;
      }
      پرلپسې_vpi_callbacks();
      أرجع PROP;
}

کينډۍ <صنف T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_ننوت_ماسک(ثابت T&val, ثابت T&force, T&rep) ثابت
{
      إذا (test_force_ماسک_is_zero())
	    أرجع PROP;

      أكد(force_ماسک_2.size() == force.size());

      rep = val;
      لكل (بینښه idx = 0 ; idx < val.size() ; idx += 1) {
	    إذا (force_ماسک_2.value(idx))
		  rep.سېټ_بټ(idx, force.value(idx));
      }

      أرجع REPL;
}

vvp_signal_ارزښت::~vvp_signal_ارزښت()
{
}

مزدوج vvp_signal_ارزښت::real_ارزښت() ثابت
{
      أكد(0);
      أرجع 0;
}

فراغ vvp_net_t::force_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&ماسک)
{
      أكد(fil);
      fil->force_fil_vec4(val, ماسک);
      fun->force_flag(خطأ);
      vvp_send_vec4(بهر_2, val, 0);
}

فراغ vvp_net_t::force_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&ماسک)
{
      أكد(fil);
      fil->force_fil_vec8(val, ماسک);
      fun->force_flag(خطأ);
      vvp_send_vec8(بهر_2, val);
}

فراغ vvp_net_t::force_real(مزدوج val, ثابت vvp_vector2_t&ماسک)
{
      أكد(fil);
      fil->force_fil_real(val, ماسک);
      fun->force_flag(خطأ);
      vvp_send_real(بهر_2, val, 0);
}

/* **** vvp_fun_signal methods **** */

vvp_fun_signal_بنسټ::vvp_fun_signal_بنسټ()
{
      continuous_ټاکل_فعال = خطأ;
      needs_init_ = ریښتیا;
      cassign_link = 0;
      شمېر_functors_sig += 1;
}

vvp_fun_signal4_sa::vvp_fun_signal4_sa(بینښه wid, vvp_bit4_t init)
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
فراغ vvp_fun_signal4_sa::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بټ,
                                   vvp_context_t)
{
      بدل (ptr.پورټ()) {
	  حالة 0: // Normal input (feed from net, or set from process)
	      /* If we don't have a continuous assign mask then just
		 copy the bits, otherwise we need to see if there are
		 any holes in the mask so we can set those bits. */
	    إذا (ټاکل_ماسک.size() == 0) {
                  إذا (needs_init_ || !bits4_.eeq(بټ)) {
			أكد(بټ.size() == bits4_.size());
			bits4_ = بټ;
			needs_init_ = خطأ;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    } وإلا {
		  منطقي بدل_شوی = خطأ;
		  أكد(bits4_.size() == ټاکل_ماسک.size());
		  لكل (بینښه idx = 0 ;  idx < بټ.size() ;  idx += 1) {
			إذا (idx >= bits4_.size()) اكسر;
			إذا (ټاکل_ماسک.value(idx)) استمر;
			bits4_.سېټ_بټ(idx, بټ.value(idx));
			بدل_شوی = ریښتیا;
		  }
		  إذا (بدل_شوی) {
			needs_init_ = خطأ;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    }
	    اكسر;

	  حالة 1: // Continuous assign value
	      // Handle the simple case of the linked source being wider
	      // than this signal. Note we don't yet support the case of
	      // the linked source being narrower than this signal, or
	      // the case of an expression being assigned.
	    bits4_ = coerce_منزل_پلنوالی(بټ, bits4_.size());
	    ټاکل_ماسک = vvp_vector2_t(vvp_vector2_t::FILL1, bits4_.size());
	    ptr.ptr()->send_vec4(bits4_, 0);
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.پورټ());
	    أكد(0);
	    اكسر;
      }
}

فراغ vvp_fun_signal4_sa::recv_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&بټ)
{
      recv_vec4(ptr, reduce4(بټ), 0);
}

فراغ vvp_fun_signal4_sa::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بټ,
				      بینښه بنسټ, بینښه vwid, vvp_context_t)
{
      أكد(bits4_.size() == vwid);
      بینښه wid = بټ.size();

      بدل (ptr.پورټ()) {
	  حالة 0: // Normal input
	    إذا (ټاکل_ماسک.size() == 0) {
                  لكل (بینښه idx = 0 ;  idx < wid ;  idx += 1) {
			إذا (بنسټ+idx >= bits4_.size()) اكسر;
			bits4_.سېټ_بټ(بنسټ+idx, بټ.value(idx));
		  }
		  needs_init_ = خطأ;
		  ptr.ptr()->send_vec4(bits4_,0);
	    } وإلا {
		  منطقي بدل_شوی = خطأ;
		  أكد(bits4_.size() == ټاکل_ماسک.size());
		  لكل (بینښه idx = 0 ;  idx < wid ;  idx += 1) {
			إذا (بنسټ+idx >= bits4_.size()) اكسر;
			إذا (ټاکل_ماسک.value(بنسټ+idx)) استمر;
			bits4_.سېټ_بټ(بنسټ+idx, بټ.value(idx));
			بدل_شوی = ریښتیا;
		  }
		  إذا (بدل_شوی) {
			needs_init_ = خطأ;
			ptr.ptr()->send_vec4(bits4_,0);
		  }
	    }
	    اكسر;

	  حالة 1: // Continuous assign value
	    إذا (ټاکل_ماسک.size() == 0)
		  ټاکل_ماسک = vvp_vector2_t(vvp_vector2_t::FILL0, bits4_.size());
	    لكل (بینښه idx = 0 ;  idx < wid ;  idx += 1) {
		  إذا (بنسټ+idx >= bits4_.size())
			اكسر;
		  bits4_.سېټ_بټ(بنسټ+idx, بټ.value(idx));
		  ټاکل_ماسک.سېټ_بټ(بنسټ+idx, 1);
	    }
	    ptr.ptr()->send_vec4(bits4_,0);
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.پورټ());
	    أكد(0);
	    اكسر;
      }
}

فراغ vvp_fun_signal4_sa::recv_vec8_pv(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&بټ,
				      بینښه بنسټ, بینښه vwid)
{
      recv_vec4_pv(ptr, reduce4(بټ), بنسټ, vwid, 0);
}

فراغ vvp_fun_signal_بنسټ::deassign()
{
      continuous_ټاکل_فعال = خطأ;
      ټاکل_ماسک = vvp_vector2_t();
}

فراغ vvp_fun_signal_بنسټ::deassign_pv(بینښه بنسټ, بینښه wid)
{
      لكل (بینښه idx = 0 ;  idx < wid ;  idx += 1) {
	    ټاکل_ماسک.سېټ_بټ(بنسټ+idx, 0);
      }

      إذا (ټاکل_ماسک.is_zero()) {
	    ټاکل_ماسک = vvp_vector2_t();
      }
}

فراغ automatic_signal_بنسټ::release(vvp_net_ptr_t,منطقي)
{
      أكد(0);
}

فراغ automatic_signal_بنسټ::release_pv(vvp_net_ptr_t,بینښه,بینښه,منطقي)
{
      أكد(0);
}

بینښه automatic_signal_بنسټ::filter_اندازه() ثابت
{
      أكد(0);
      أرجع(0);
}
فراغ automatic_signal_بنسټ::force_fil_vec4(ثابت vvp_vector4_t&, ثابت vvp_vector2_t&)
{
      أكد(0);
}
فراغ automatic_signal_بنسټ::force_fil_vec8(ثابت vvp_vector8_t&, ثابت vvp_vector2_t&)
{
      أكد(0);
}
فراغ automatic_signal_بنسټ::force_fil_real(مزدوج, ثابت vvp_vector2_t&)
{
      أكد(0);
}
فراغ automatic_signal_بنسټ::get_ارزښت(بنية t_vpi_ارزښت*)
{
      أكد(0);
}

ثابت vvp_vector4_t& vvp_fun_signal4_sa::vec4_unfiltered_ارزښت() ثابت
{
      أرجع bits4_;
}

vvp_fun_signal4_aa::vvp_fun_signal4_aa(بینښه wid, vvp_bit4_t init)
{
      context_idx_ = vpip_جمع_کړه_توکی_منزل_context(هذا, vpip_peek_context_scope());
      اندازه_2 = wid;
      init_ = init;
}

vvp_fun_signal4_aa::~vvp_fun_signal4_aa()
{
      أكد(0);
}

فراغ vvp_fun_signal4_aa::alloc_instance(vvp_context_t context)
{
      vvp_سېټ_context_توکی(context, context_idx_, جديد vvp_vector4_t(اندازه_2, init_));
}

فراغ vvp_fun_signal4_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*بټونه = static_cast<vvp_vector4_t*>
            (vvp_get_context_توکی(context, context_idx_));

      بټونه->fill_بټونه(init_);
}

#که_تعریف_شوی CHECK_WITH_VALGRIND
فراغ vvp_fun_signal4_aa::free_instance(vvp_context_t context)
{
      vvp_vector4_t*بټونه = static_cast<vvp_vector4_t*>
            (vvp_get_context_توکی(context, context_idx_));
      ړنګول بټونه;
}
#د_که_پای

/*
 * Continuous and forced assignments are not permitted on automatic
 * variables. So we only expect to receive on port 0.
 */
فراغ vvp_fun_signal4_aa::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بټ,
                                   vvp_context_t context)
{
      أكد(ptr.پورټ() == 0);
      أكد(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_توکی(context, context_idx_));

      إذا (!bits4->eeq(بټ)) {
            *bits4 = بټ;
            ptr.ptr()->send_vec4(*bits4, context);
      }
}

فراغ vvp_fun_signal4_aa::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بټ,
				      بینښه بنسټ, بینښه vwid, vvp_context_t context)
{
      أكد(ptr.پورټ() == 0);
      أكد(اندازه_2 == vwid);
      أكد(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_توکی(context, context_idx_));

      بینښه wid = بټ.size();
      لكل (بینښه idx = 0 ;  idx < wid ;  idx += 1) {
            إذا (بنسټ+idx >= bits4->size()) اكسر;
            bits4->سېټ_بټ(بنسټ+idx, بټ.value(idx));
      }
      ptr.ptr()->send_vec4(*bits4, context);
}

بینښه vvp_fun_signal4_aa::ارزښت_اندازه() ثابت
{
      أرجع اندازه_2;
}

vvp_bit4_t vvp_fun_signal4_aa::value(بینښه idx) ثابت
{
      ثابت vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_توکی(context_idx_));

      أرجع bits4->value(idx);
}

vvp_scalar_t vvp_fun_signal4_aa::scalar_ارزښت(بینښه idx) ثابت
{
      ثابت vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_توکی(context_idx_));

      أرجع vvp_scalar_t(bits4->value(idx), 6, 6);
}

فراغ vvp_fun_signal4_aa::vec4_ارزښت(vvp_vector4_t&val) ثابت
{
      ثابت vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_توکی(context_idx_));

      val = *bits4;
}

ثابت vvp_vector4_t&vvp_fun_signal4_aa::vec4_unfiltered_ارزښت() ثابت
{
      ثابت vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_توکی(context_idx_));

      أرجع *bits4;
}

فراغ vvp_fun_signal4_aa::چلوونکی ړنګول(فراغ*)
{
      أكد(0);
}

/*
 * Testing for equality, we want a bitwise test instead of an
 * arithmetic test because we want to treat for example -0 different
 * from +0.
 */
منطقي بټونه_equal(مزدوج a, مزدوج b)
{
      أرجع memcmp(&a, &b, حجم a) == 0;
}

vvp_fun_signal_real_sa::vvp_fun_signal_real_sa()
{
      بټونه_2 = 0.0;
}

مزدوج vvp_fun_signal_real_sa::real_unfiltered_ارزښت() ثابت
{
      أرجع بټونه_2;
}

فراغ vvp_fun_signal_real_sa::recv_real(vvp_net_ptr_t ptr, مزدوج بټ,
                                       vvp_context_t)
{
      بدل (ptr.پورټ()) {
	  حالة 0:
	    إذا (!continuous_ټاکل_فعال) {
                  إذا (needs_init_ || !بټونه_equal(بټونه_2, بټ)) {
			بټونه_2 = بټ;
			needs_init_ = خطأ;
			ptr.ptr()->send_real(بټ, 0);
		  }
	    }
	    اكسر;

	  حالة 1: // Continuous assign value
	    continuous_ټاکل_فعال = ریښتیا;
	    بټونه_2 = بټ;
	    ptr.ptr()->send_real(بټ, 0);
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.پورټ());
	    أكد(0);
	    اكسر;
      }
}

vvp_fun_signal_real_aa::vvp_fun_signal_real_aa()
{
      context_idx_ = vpip_جمع_کړه_توکی_منزل_context(هذا, vpip_peek_context_scope());
}

vvp_fun_signal_real_aa::~vvp_fun_signal_real_aa()
{
      أكد(0);
}

فراغ vvp_fun_signal_real_aa::alloc_instance(vvp_context_t context)
{
      مزدوج*بټونه = جديد مزدوج;
      vvp_سېټ_context_توکی(context, context_idx_, بټونه);

      *بټونه = 0.0;
}

فراغ vvp_fun_signal_real_aa::reset_instance(vvp_context_t context)
{
      مزدوج*بټونه = static_cast<مزدوج*>
            (vvp_get_context_توکی(context, context_idx_));

      *بټونه = 0.0;
}

#که_تعریف_شوی CHECK_WITH_VALGRIND
فراغ vvp_fun_signal_real_aa::free_instance(vvp_context_t context)
{
      مزدوج*بټونه = static_cast<مزدوج*>
            (vvp_get_context_توکی(context, context_idx_));
      ړنګول بټونه;
}
#د_که_پای

مزدوج vvp_fun_signal_real_aa::real_unfiltered_ارزښت() ثابت
{
      ثابت مزدوج*بټونه = static_cast<مزدوج*>
            (vthread_get_rd_context_توکی(context_idx_));

      أرجع *بټونه;
}

مزدوج vvp_fun_signal_real_aa::real_ارزښت() ثابت
{
      أرجع real_unfiltered_ارزښت();
}

فراغ vvp_fun_signal_real_aa::recv_real(vvp_net_ptr_t ptr, مزدوج بټ,
                                       vvp_context_t context)
{
      أكد(ptr.پورټ() == 0);
      أكد(context);

      مزدوج*بټونه = static_cast<مزدوج*>
            (vvp_get_context_توکی(context, context_idx_));

      إذا (!بټونه_equal(*بټونه,بټ)) {
            *بټونه = بټ;
            ptr.ptr()->send_real(بټ, context);
      }
}

بینښه vvp_fun_signal_real_aa::ارزښت_اندازه() ثابت
{
      أكد(0);
      أرجع 1;
}

vvp_bit4_t vvp_fun_signal_real_aa::value(بینښه) ثابت
{
      أكد(0);
      أرجع BIT4_X;
}

vvp_scalar_t vvp_fun_signal_real_aa::scalar_ارزښت(بینښه) ثابت
{
      أكد(0);
      أرجع vvp_scalar_t();
}

فراغ vvp_fun_signal_real_aa::vec4_ارزښت(vvp_vector4_t&) ثابت
{
      أكد(0);
}

فراغ* vvp_fun_signal_real_aa::چلوونکی جديد(std::size_t size)
{
      أرجع vvp_net_fun_t::ډېرۍ.alloc(size);
}

فراغ vvp_fun_signal_real_aa::چلوونکی ړنګول(فراغ*)
{
      أكد(0);
}


vvp_fun_signal_string_sa::vvp_fun_signal_string_sa()
{
}

فراغ vvp_fun_signal_string_sa::recv_string(vvp_net_ptr_t ptr, ثابت std::string&بټ,
					   vvp_context_t)
{
      أكد(ptr.پورټ() == 0);

      إذا (needs_init_ || ارزښت_2 != بټ) {
	    ارزښت_2 = بټ;
	    needs_init_ = خطأ;

	    ptr.ptr()->send_string(بټ, 0);
      }
}

ثابت string& vvp_fun_signal_string_sa::get_string() ثابت
{
      أرجع ارزښت_2;
}

vvp_fun_signal_string_aa::vvp_fun_signal_string_aa()
{
      context_idx_ = vpip_جمع_کړه_توکی_منزل_context(هذا, vpip_peek_context_scope());
}

vvp_fun_signal_string_aa::~vvp_fun_signal_string_aa()
{
      أكد(0);
}

فراغ vvp_fun_signal_string_aa::alloc_instance(vvp_context_t context)
{
      string*بټونه = جديد std::string;
      vvp_سېټ_context_توکی(context, context_idx_, بټونه);
      *بټونه = "";
}

فراغ vvp_fun_signal_string_aa::reset_instance(vvp_context_t context)
{
      string*بټونه = static_cast<std::string*>
	    (vvp_get_context_توکی(context, context_idx_));
      *بټونه = "";
}

#که_تعریف_شوی CHECK_WITH_VALGRIND
فراغ vvp_fun_signal_string_aa::free_instance(vvp_context_t context)
{
      string*بټونه = static_cast<std::string*>
            (vvp_get_context_توکی(context, context_idx_));
      ړنګول بټونه;
}
#د_که_پای

فراغ vvp_fun_signal_string_aa::recv_string(vvp_net_ptr_t ptr, ثابت std::string&بټ, vvp_context_t context)
{
      أكد(ptr.پورټ() == 0);
      أكد(context);

      string*بټونه = static_cast<std::string*>
	    (vvp_get_context_توکی(context, context_idx_));

      إذا (*بټونه != بټ) {
	    *بټونه = بټ;
	    ptr.ptr()->send_string(بټ, context);
      }
}

بینښه vvp_fun_signal_string_aa::ارزښت_اندازه() ثابت
{
      أكد(0);
      أرجع 1;
}

vvp_bit4_t vvp_fun_signal_string_aa::value(بینښه) ثابت
{
      أكد(0);
      أرجع BIT4_X;
}

vvp_scalar_t vvp_fun_signal_string_aa::scalar_ارزښت(بینښه) ثابت
{
      أكد(0);
      أرجع vvp_scalar_t();
}

فراغ vvp_fun_signal_string_aa::vec4_ارزښت(vvp_vector4_t&) ثابت
{
      أكد(0);
}

مزدوج vvp_fun_signal_string_aa::real_ارزښت() ثابت
{
      أكد(0);
      أرجع 0.0;
}

ثابت std::string& vvp_fun_signal_string_aa::get_string() ثابت
{
      ثابت string*بټونه = static_cast<std::string*>
            (vthread_get_rd_context_توکی(context_idx_));

      أرجع *بټونه;
}

فراغ* vvp_fun_signal_string_aa::چلوونکی جديد(std::size_t size)
{
      أرجع vvp_net_fun_t::ډېرۍ.alloc(size);
}

فراغ vvp_fun_signal_string_aa::چلوونکی ړنګول(فراغ*)
{
      أكد(0);
}

  /* OBJECT signals */

vvp_fun_signal_object_sa::vvp_fun_signal_object_sa(بینښه size)
: vvp_fun_signal_object(size)
{
}

#که_تعریف_شوی CHECK_WITH_VALGRIND
فراغ vvp_fun_signal_object_aa::free_instance(vvp_context_t context)
{
      vvp_object_t*بټونه = static_cast<vvp_object_t*>
            (vvp_get_context_توکی(context, context_idx_));
      ړنګول بټونه;
}
#د_که_پای

فراغ vvp_fun_signal_object_sa::recv_object(vvp_net_ptr_t ptr, vvp_object_t بټ,
					   vvp_context_t)
{
      أكد(ptr.پورټ() == 0);

      إذا (needs_init_ || ارزښت_2 != بټ) {
	    ارزښت_2 = بټ;
	    needs_init_ = خطأ;

	    ptr.ptr()->send_object(بټ, 0);
      }
}

vvp_object_t vvp_fun_signal_object_sa::get_object() ثابت
{
      أرجع ارزښت_2;
}

vvp_fun_signal_object_aa::vvp_fun_signal_object_aa(بینښه size)
: vvp_fun_signal_object(size)
{
      context_idx_ = vpip_جمع_کړه_توکی_منزل_context(هذا, vpip_peek_context_scope());
}

vvp_fun_signal_object_aa::~vvp_fun_signal_object_aa()
{
      أكد(0);
}

فراغ vvp_fun_signal_object_aa::alloc_instance(vvp_context_t context)
{
      vvp_object_t*بټونه = جديد vvp_object_t;
      vvp_سېټ_context_توکی(context, context_idx_, بټونه);
      بټونه->reset();
}

فراغ vvp_fun_signal_object_aa::reset_instance(vvp_context_t context)
{
      vvp_object_t*بټونه = static_cast<vvp_object_t*>
	    (vvp_get_context_توکی(context, context_idx_));
      بټونه->reset();
}

vvp_object_t vvp_fun_signal_object_aa::get_object() ثابت
{
      ثابت vvp_object_t*بټونه = static_cast<vvp_object_t*>
	    (vthread_get_rd_context_توکی(context_idx_));
      أرجع *بټونه;
}

فراغ vvp_fun_signal_object_aa::recv_object(vvp_net_ptr_t ptr, vvp_object_t بټ,
					   vvp_context_t context)
{
      أكد(ptr.پورټ() == 0);
      أكد(context);

      vvp_object_t*بټونه = static_cast<vvp_object_t*>
	    (vvp_get_context_توکی(context, context_idx_));

      إذا (*بټونه != بټ) {
	    *بټونه = بټ;
	    ptr.ptr()->send_object(بټ, context);
      }
}

بینښه vvp_fun_signal_object_aa::ارزښت_اندازه() ثابت
{
      أرجع 1;
}

vvp_bit4_t vvp_fun_signal_object_aa::value(بینښه) ثابت
{
      أكد(0);
      أرجع BIT4_X;
}

vvp_scalar_t vvp_fun_signal_object_aa::scalar_ارزښت(بینښه) ثابت
{
      أكد(0);
      أرجع vvp_scalar_t();
}

فراغ vvp_fun_signal_object_aa::vec4_ارزښت(vvp_vector4_t&) ثابت
{
      أكد(0);
}

فراغ* vvp_fun_signal_object_aa::چلوونکی جديد(std::size_t size)
{
      أرجع vvp_net_fun_t::ډېرۍ.alloc(size);
}

فراغ vvp_fun_signal_object_aa::چلوونکی ړنګول(فراغ*)
{
      أكد(0);
}

  /* **** */

vvp_fun_force::vvp_fun_force()
{
}

vvp_fun_force::~vvp_fun_force()
{
}

فراغ vvp_fun_force::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بټ,
			      vvp_context_t)
{
      أكد(ptr.پورټ() == 0);
      vvp_net_t*net = ptr.ptr();

      vvp_net_t*dst = net->پورټ[3].ptr();
      أكد(dst->fil);

      dst->force_vec4(coerce_منزل_پلنوالی(بټ, dst->fil->filter_اندازه()), vvp_vector2_t(vvp_vector2_t::FILL1, dst->fil->filter_اندازه()));
}

فراغ vvp_fun_force::recv_real(vvp_net_ptr_t ptr, مزدوج بټ, vvp_context_t)
{
      أكد(ptr.پورټ() == 0);
      vvp_net_t*net = ptr.ptr();
      vvp_net_t*dst = net->پورټ[3].ptr();
      dst->force_real(بټ, vvp_vector2_t(vvp_vector2_t::FILL1, 1));
}

vvp_wire_بنسټ::vvp_wire_بنسټ()
{
}

vvp_wire_بنسټ::~vvp_wire_بنسټ()
{
}

vvp_bit4_t vvp_wire_بنسټ::driven_ارزښت(بینښه) ثابت
{
      أكد(0);
      أرجع BIT4_X;
}

منطقي vvp_wire_بنسټ::is_forced(بینښه) ثابت
{
      أكد(0);
      أرجع خطأ;
}

vvp_wire_vec4::vvp_wire_vec4(بینښه wid, vvp_bit4_t init)
: bits4_(wid, init)
{
      needs_init_ = ریښتیا;
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec4(ثابت vvp_vector4_t&بټ, vvp_vector4_t&rep,
						 بینښه بنسټ, بینښه vwid)
{
	// Special case! the input bit is 0 wid. Interpret this as a
	// vector of BIT4_X to match the width of the bits4_ vector.
	// FIXME! This is a hack to work around some buggy gate
	// implementations! This should be removed!
      إذا (بنسټ==0 && vwid==0) {
	    vvp_vector4_t tmp (bits4_.size(), BIT4_X);
	    إذا (bits4_ .eeq(tmp) && !needs_init_) أرجع STOP;
	    bits4_ = tmp;
	    needs_init_ = خطأ;
	    أرجع filter_ماسک(tmp, force4_, rep, 0);
      }

      إذا (vwid != bits4_.size()) {
	    cerr << "Internal error: Input vector expected width="
		 << bits4_.size() << ", got "
		 << "bit=" << بټ << ", base=" << بنسټ << ", vwid=" << vwid
		 << کرښه_پای;
      }
      أكد(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      إذا (بنسټ==0 && بټ.size()==vwid) {
	    إذا (bits4_ .eeq( بټ ) && !needs_init_) أرجع STOP;
	    bits4_ = بټ;
      } وإلا {
	    منطقي rc = bits4_.سېټ_vec(بنسټ, بټ);
	    إذا (rc == خطأ && !needs_init_) أرجع STOP;
      }

      needs_init_ = خطأ;
      أرجع filter_ماسک(بټ, force4_, rep, بنسټ);
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec8(ثابت vvp_vector8_t&بټ,
                                                 vvp_vector8_t&rep,
                                                 بینښه بنسټ,
                                                 بینښه vwid)
{
      أكد(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      vvp_vector4_t bit4 (reduce4(بټ));
      إذا (بنسټ==0 && bit4.size()==vwid) {
	    إذا (bits4_ .eeq( bit4 ) && !needs_init_) أرجع STOP;
	    bits4_ = bit4;
      } وإلا {
	    منطقي rc = bits4_.سېټ_vec(بنسټ, bit4);
	    إذا (rc == خطأ && !needs_init_) أرجع STOP;
      }

      needs_init_ = خطأ;
      أرجع filter_ماسک(بټ, vvp_vector8_t(force4_,6,6), rep, بنسټ);
}

بینښه vvp_wire_vec4::filter_اندازه() ثابت
{
      أرجع bits4_.size();
}

فراغ vvp_wire_vec4::force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&ماسک)
{
      force_ماسک(ماسک);

      إذا (force4_.size() == 0) {
	    force4_ = val;
      } وإلا {
	    لكل (بینښه idx = 0; idx < ماسک.size() ; idx += 1) {
		  إذا (ماسک.value(idx) == 0)
			استمر;

		  force4_.سېټ_بټ(idx, val.value(idx));
	    }
      }
      پرلپسې_vpi_callbacks();
}

فراغ vvp_wire_vec4::force_fil_vec8(ثابت vvp_vector8_t&, ثابت vvp_vector2_t&)
{
      أكد(0);
}

فراغ vvp_wire_vec4::force_fil_real(مزدوج, ثابت vvp_vector2_t&)
{
      أكد(0);
}

فراغ vvp_wire_vec4::release(vvp_net_ptr_t ptr, منطقي net_flag)
{
      vvp_vector2_t ماسک (vvp_vector2_t::FILL1, bits4_.size());
      إذا (net_flag) {
	      // Wires revert to their unforced value after release.
            release_ماسک(ماسک);
	    needs_init_ = ! force4_ .eeq(bits4_);
	    ptr.ptr()->send_vec4(bits4_, 0);
	    پرلپسې_vpi_callbacks();
      } وإلا {
	      // Variables keep the current value.
	    vvp_vector4_t res (bits4_.size());
	    لكل (بینښه idx=0; idx<bits4_.size(); idx += 1)
		  res.سېټ_بټ(idx,value(idx));
            release_ماسک(ماسک);
	    ptr.ptr()->fun->recv_vec4(ptr, res, 0);
      }
}

فراغ vvp_wire_vec4::release_pv(vvp_net_ptr_t ptr, بینښه بنسټ, بینښه wid, منطقي net_flag)
{
      أكد(bits4_.size() >= بنسټ + wid);

      vvp_vector2_t ماسک (vvp_vector2_t::FILL0, bits4_.size());
      لكل (بینښه idx = 0 ; idx < wid ; idx += 1)
	    ماسک.سېټ_بټ(بنسټ+idx, 1);

      إذا (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_ماسک(ماسک);
	    needs_init_ = ! force4_.subvalue(بنسټ,wid) .eeq(bits4_.subvalue(بنسټ,wid));
	    ptr.ptr()->send_vec4_pv(bits4_.subvalue(بنسټ,wid),
				    بنسټ, bits4_.size(), 0);
	    پرلپسې_vpi_callbacks();
      } وإلا {
	      // Variables keep the current value.
	    vvp_vector4_t res (wid);
	    لكل (بینښه idx=0; idx<wid; idx += 1)
		  res.سېټ_بټ(idx,value(بنسټ+idx));
	    release_ماسک(ماسک);
	    ptr.ptr()->fun->recv_vec4_pv(ptr, res, بنسټ, bits4_.size(), 0);
      }
}

بینښه vvp_wire_vec4::ارزښت_اندازه() ثابت
{
      أرجع bits4_.size();
}

vvp_bit4_t vvp_wire_vec4::filtered_ارزښت(بینښه idx) ثابت
{
      إذا (test_force_ماسک(idx))
	    أرجع force4_.value(idx);
      وإلا
	    أرجع bits4_.value(idx);
}

vvp_bit4_t vvp_wire_vec4::value(بینښه idx) ثابت
{
      أرجع filtered_ارزښت(idx);
}

vvp_scalar_t vvp_wire_vec4::scalar_ارزښت(بینښه idx) ثابت
{
      أرجع vvp_scalar_t(value(idx),6,6);
}

فراغ vvp_wire_vec4::vec4_ارزښت(vvp_vector4_t&val) ثابت
{
      val = bits4_;
      إذا (test_force_ماسک_is_zero())
	    أرجع;

      لكل (بینښه idx = 0 ; idx < bits4_.size() ; idx += 1)
	    val.سېټ_بټ(idx, filtered_ارزښت(idx));
}

vvp_bit4_t vvp_wire_vec4::driven_ارزښت(بینښه idx) ثابت
{
      أرجع bits4_.value(idx);
}

منطقي vvp_wire_vec4::is_forced(بینښه idx) ثابت
{
      أرجع test_force_ماسک(idx);
}

vvp_wire_vec8::vvp_wire_vec8(بینښه wid)
: bits8_(wid)
{
      needs_init_ = ریښتیا;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec4(ثابت vvp_vector4_t&بټ,
                                                 vvp_vector4_t&rep,
                                                 بینښه بنسټ,
                                                 بینښه vwid)
{
	// For now there is no support for a non-zero base.
      أكد(0 == بنسټ);
      أكد(bits8_.size() == vwid);
      أكد(bits8_.size() == بټ.size());
	// QUESTION: Is it really correct to propagate a vec4 if this
	// is a vec8 node? In fact, it is really possible for a vec4
	// value to get through to a vec8 filter?
      vvp_vector8_t rep8;
      prop_t rc = filter_vec8(vvp_vector8_t(بټ,6,6), rep8, 0, vwid);
      إذا (rc == REPL)
	    rep = reduce4(rep8);

      needs_init_ = خطأ;
      أرجع rc;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec8(ثابت vvp_vector8_t&بټ, vvp_vector8_t&rep, بینښه بنسټ, بینښه vwid)
{
      أكد(vwid == bits8_.size());
	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      إذا (بنسټ==0 && بټ.size()==vwid) {
	    إذا (bits8_.eeq(بټ) && !needs_init_) أرجع STOP;
	    bits8_ = بټ;
      } وإلا {
	    إذا (bits8_.subvalue(بنسټ, بټ.size()).eeq(بټ) &&
		!needs_init_) {
		  أرجع STOP;
	    }
	    إذا (bits8_.size() == 0)
		  bits8_ = vvp_vector8_t(vwid);
	    أكد(bits8_.size() == vwid);
	    bits8_.سېټ_vec(بنسټ, بټ);
      }
      needs_init_ = خطأ;
      أرجع filter_ماسک(بټ, force8_, rep, بنسټ);
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_ننوت_vec8(ثابت vvp_vector8_t&بټ, vvp_vector8_t&rep) ثابت
{
      أرجع filter_ننوت_ماسک(بټ, force8_, rep);
}

بینښه vvp_wire_vec8::filter_اندازه() ثابت
{
      أرجع bits8_.size();
}

فراغ vvp_wire_vec8::force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&ماسک)
{
      force_fil_vec8(vvp_vector8_t(val,6,6), ماسک);
}

فراغ vvp_wire_vec8::force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&ماسک)
{
      force_ماسک(ماسک);

      إذا (force8_.size() == 0) {
	    force8_ = val;
      } وإلا {
	    لكل (بینښه idx = 0; idx < ماسک.size() ; idx += 1) {
		  إذا (ماسک.value(idx) == 0)
			استمر;

		  force8_.سېټ_بټ(idx, val.value(idx));
	    }
      }
      پرلپسې_vpi_callbacks();
}

فراغ vvp_wire_vec8::force_fil_real(مزدوج, ثابت vvp_vector2_t&)
{
      أكد(0);
}

فراغ vvp_wire_vec8::release(vvp_net_ptr_t ptr, منطقي net_flag)
{
	// Wires revert to their unforced value after release.
      vvp_vector2_t ماسک (vvp_vector2_t::FILL1, bits8_.size());
      release_ماسک(ماسک);
      إذا (net_flag) {
	    needs_init_ = !force8_ .eeq(bits8_);
	    ptr.ptr()->send_vec8(bits8_);
      } وإلا {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    أكد(0);
//	    ptr.ptr()->fun->recv_vec8(ptr, force8_);
      }
}

فراغ vvp_wire_vec8::release_pv(vvp_net_ptr_t ptr, بینښه بنسټ, بینښه wid, منطقي net_flag)
{
      أكد(bits8_.size() >= بنسټ + wid);

      vvp_vector2_t ماسک (vvp_vector2_t::FILL0, bits8_.size());
      لكل (بینښه idx = 0 ; idx < wid ; idx += 1)
	    ماسک.سېټ_بټ(بنسټ+idx, 1);

      release_ماسک(ماسک);

      إذا (net_flag) {
	    needs_init_ = !force8_.subvalue(بنسټ,wid) .eeq((bits8_.subvalue(بنسټ,wid)));
	    ptr.ptr()->send_vec8_pv(bits8_.subvalue(بنسټ,wid),
				    بنسټ, bits8_.size());
	    پرلپسې_vpi_callbacks();
      } وإلا {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    أكد(0);
//	    ptr.ptr()->fun->recv_vec8_pv(ptr, force8_.subvalue(base,wid),
//					 base, force8_.size());
      }
}

بینښه vvp_wire_vec8::ارزښت_اندازه() ثابت
{
      أرجع bits8_.size();
}

vvp_scalar_t vvp_wire_vec8::filtered_ارزښت(بینښه idx) ثابت
{
      إذا (test_force_ماسک(idx))
	    أرجع force8_.value(idx);
      وإلا
	    أرجع bits8_.value(idx);
}

vvp_bit4_t vvp_wire_vec8::value(بینښه idx) ثابت
{
      أرجع filtered_ارزښت(idx).value();
}

vvp_scalar_t vvp_wire_vec8::scalar_ارزښت(بینښه idx) ثابت
{
      أرجع filtered_ارزښت(idx);
}

vvp_vector8_t vvp_wire_vec8::vec8_ارزښت() ثابت
{
      vvp_vector8_t tmp = bits8_;
      لكل (بینښه idx = 0 ; idx < bits8_.size() ; idx += 1)
	    tmp.سېټ_بټ(idx, filtered_ارزښت(idx));
      أرجع tmp;
}

فراغ vvp_wire_vec8::vec4_ارزښت(vvp_vector4_t&val) ثابت
{
      val = reduce4(vec8_ارزښت());
}

vvp_bit4_t vvp_wire_vec8::driven_ارزښت(بینښه idx) ثابت
{
      أرجع bits8_.value(idx).value();
}

منطقي vvp_wire_vec8::is_forced(بینښه idx) ثابت
{
      أرجع test_force_ماسک(idx);
}

vvp_wire_real::vvp_wire_real()
: بټ_2(0.0), force_(0.0)
{
}

vvp_net_fil_t::prop_t vvp_wire_real::filter_real(مزدوج&بټ)
{
      إذا (بټونه_equal(بټ_2, بټ) && !needs_init_) أرجع STOP;
      بټ_2 = بټ;
      needs_init_ = خطأ;
      أرجع filter_ماسک(بټ, force_);
}

بینښه vvp_wire_real::filter_اندازه() ثابت
{
      أكد(0);
      أرجع 0;
}

فراغ vvp_wire_real::force_fil_vec4(ثابت vvp_vector4_t&, ثابت vvp_vector2_t&)
{
      أكد(0);
}

فراغ vvp_wire_real::force_fil_vec8(ثابت vvp_vector8_t&, ثابت vvp_vector2_t&)
{
      أكد(0);
}

فراغ vvp_wire_real::force_fil_real(مزدوج val, ثابت vvp_vector2_t&ماسک)
{
      force_ماسک(ماسک);
      إذا (ماسک.value(0))
	    force_ = val;

      پرلپسې_vpi_callbacks();
}

فراغ vvp_wire_real::release(vvp_net_ptr_t ptr, منطقي net_flag)
{
      vvp_vector2_t ماسک (vvp_vector2_t::FILL1, 1);
      إذا (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_ماسک(ماسک);
	    needs_init_ = !بټونه_equal(force_, بټ_2);
	    ptr.ptr()->send_real(بټ_2, 0);
      } وإلا {
	      // Variables keep the current value.
	    مزدوج res =  real_ارزښت();
	    release_ماسک(ماسک);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
}

فراغ vvp_wire_real::release_pv(vvp_net_ptr_t, بینښه, بینښه, منطقي)
{
      أكد(0);
#إذا 0
	// A real is a single value. If for some reason this part release
	// can happen the following code should work correctly (requires
	// a base of 0 and a width of 1).
      vvp_vector2_t ماسک (vvp_vector2_t::FILL1, 1);
      أكد(بنسټ == 0 && wid == 1);

      إذا (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_ماسک(ماسک);
	    ptr.ptr()->send_real(بټ_2, 0);
      } وإلا {
	      // Variables keep the current value.
	    مزدوج res =  real_ارزښت();
	    release_ماسک(ماسک);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
#د_که_پای
}

بینښه vvp_wire_real::ارزښت_اندازه() ثابت
{
      أكد(0);
      أرجع 1;
}

vvp_bit4_t vvp_wire_real::value(بینښه) ثابت
{
      أكد(0);
      أرجع BIT4_X;
}

vvp_scalar_t vvp_wire_real::scalar_ارزښت(بینښه) ثابت
{
      أكد(0);
      أرجع vvp_scalar_t();
}

فراغ vvp_wire_real::vec4_ارزښت(vvp_vector4_t&) ثابت
{
      أكد(0);
}

مزدوج vvp_wire_real::real_ارزښت() ثابت
{
      إذا (test_force_ماسک(0))
	    أرجع force_;
      وإلا
	    أرجع بټ_2;
}

#إذا 0
vvp_wire_string::vvp_wire_string()
{
}

بینښه vvp_wire_string::filter_اندازه() ثابت
{
      أكد(0);
      أرجع 0;
}

فراغ vvp_wire_string::force_fil_vec4(ثابت vvp_vector4_t&, ثابت vvp_vector2_t&)
{
      أكد(0);
}
فراغ vvp_wire_string::force_fil_vec8(ثابت vvp_vector8_t&, ثابت vvp_vector2_t&)
{
      أكد(0);
}
فراغ vvp_wire_string::force_fil_real(مزدوج, ثابت vvp_vector2_t&)
{
      أكد(0);
}

فراغ vvp_wire_string::release(vvp_net_ptr_t ptr, منطقي net_flag)
{
      أكد(0);
}

فراغ vvp_wire_string::release_pv(vvp_net_ptr_t, بینښه, بینښه, منطقي)
{
      أكد(0);
}

بینښه vvp_wire_string::ارزښت_اندازه() ثابت
{
      أكد(0);
      أرجع 1;
}

vvp_bit4_t vvp_wire_string::value(بینښه) ثابت
{
      أكد(0);
      أرجع BIT4_X;
}

vvp_scalar_t vvp_wire_string::scalar_ارزښت(بینښه) ثابت
{
      أكد(0);
      أرجع vvp_scalar_t();
}

فراغ vvp_wire_string::vec4_ارزښت(vvp_vector4_t&) ثابت
{
      أكد(0);
}

مزدوج vvp_wire_string::real_ارزښت() ثابت
{
      أكد(0);
      أرجع 0.0;
}
#د_که_پای
