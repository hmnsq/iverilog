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
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
# include  <map>
#جيڪڏهن_پورو

# include  <iostream>

استعمال نطاق std;

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
سانچو <صنف T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_نقاب(ثابت T&val, ثابت T&force, T&filter, بنانشان بنياد)
{
      إذا (!test_force_نقاب_is_zero()) {
	      // Some bits are being forced. Go through the
	      // force_mask_ and force value to see which bits are
	      // propagated and which are kept from the forced
	      // value. Update the filter with the filtered result and
	      // return REPL to indicate that some bits have changed,
	      // or STOP if no bits change.
	    منطقي propagate_flag = force_propagate_;
	    force_propagate_ = خطأ;
	    أكد(force_نقاب_2.size() == force.size());
	    أكد((بنياد+val.size()) <= force_نقاب_2.size());

	    filter = val;
	    لكل (بنانشان idx = 0 ; idx < val.size() ; idx += 1) {
		  إذا (force_نقاب_2.value(بنياد+idx))
			filter.سيٽ_ٻٽو_انگ(idx, force.value(بنياد+idx));
		  وإلا
			propagate_flag = سچ;
	    }

	    إذا (propagate_flag) {
		  لاڳيتو_vpi_callbacks();
		  أرجع REPL;
	    } وإلا {
		  أرجع STOP;
	    }

      } وإلا {
	    لاڳيتو_vpi_callbacks();
	    أرجع PROP;
      }
}

سانچو <صنف T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_نقاب(T&val, T force)
{

      إذا (test_force_نقاب(0)) {
	    val = force;
	    لاڳيتو_vpi_callbacks();
	    أرجع REPL;
      }
      لاڳيتو_vpi_callbacks();
      أرجع PROP;
}

سانچو <صنف T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_داخلا_نقاب(ثابت T&val, ثابت T&force, T&rep) ثابت
{
      إذا (test_force_نقاب_is_zero())
	    أرجع PROP;

      أكد(force_نقاب_2.size() == force.size());

      rep = val;
      لكل (بنانشان idx = 0 ; idx < val.size() ; idx += 1) {
	    إذا (force_نقاب_2.value(idx))
		  rep.سيٽ_ٻٽو_انگ(idx, force.value(idx));
      }

      أرجع REPL;
}

vvp_signal_قدر::~vvp_signal_قدر()
{
}

مزدوج vvp_signal_قدر::real_قدر() ثابت
{
      أكد(0);
      أرجع 0;
}

فراغ vvp_net_t::force_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&نقاب)
{
      أكد(fil);
      fil->force_fil_vec4(val, نقاب);
      fun->force_flag(خطأ);
      vvp_send_vec4(ٻاهريون_2, val, 0);
}

فراغ vvp_net_t::force_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&نقاب)
{
      أكد(fil);
      fil->force_fil_vec8(val, نقاب);
      fun->force_flag(خطأ);
      vvp_send_vec8(ٻاهريون_2, val);
}

فراغ vvp_net_t::force_real(مزدوج val, ثابت vvp_vector2_t&نقاب)
{
      أكد(fil);
      fil->force_fil_real(val, نقاب);
      fun->force_flag(خطأ);
      vvp_send_real(ٻاهريون_2, val, 0);
}

/* **** vvp_fun_signal methods **** */

vvp_fun_signal_بنياد::vvp_fun_signal_بنياد()
{
      continuous_سونپيو_سرگرم = خطأ;
      needs_init_ = سچ;
      cassign_link = 0;
      ڳڻپ_functors_sig += 1;
}

vvp_fun_signal4_sa::vvp_fun_signal4_sa(بنانشان wid, vvp_bit4_t init)
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
فراغ vvp_fun_signal4_sa::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ٻٽو_انگ,
                                   vvp_context_t)
{
      بدل (ptr.پورٽ()) {
	  حالة 0: // Normal input (feed from net, or set from process)
	      /* If we don't have a continuous assign mask then just
		 copy the bits, otherwise we need to see if there are
		 any holes in the mask so we can set those bits. */
	    إذا (سونپيو_نقاب.size() == 0) {
                  إذا (needs_init_ || !bits4_.eeq(ٻٽو_انگ)) {
			أكد(ٻٽو_انگ.size() == bits4_.size());
			bits4_ = ٻٽو_انگ;
			needs_init_ = خطأ;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    } وإلا {
		  منطقي بدليو = خطأ;
		  أكد(bits4_.size() == سونپيو_نقاب.size());
		  لكل (بنانشان idx = 0 ;  idx < ٻٽو_انگ.size() ;  idx += 1) {
			إذا (idx >= bits4_.size()) اكسر;
			إذا (سونپيو_نقاب.value(idx)) استمر;
			bits4_.سيٽ_ٻٽو_انگ(idx, ٻٽو_انگ.value(idx));
			بدليو = سچ;
		  }
		  إذا (بدليو) {
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
	    bits4_ = coerce_منزل_ويڪر(ٻٽو_انگ, bits4_.size());
	    سونپيو_نقاب = vvp_vector2_t(vvp_vector2_t::FILL1, bits4_.size());
	    ptr.ptr()->send_vec4(bits4_, 0);
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.پورٽ());
	    أكد(0);
	    اكسر;
      }
}

فراغ vvp_fun_signal4_sa::recv_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&ٻٽو_انگ)
{
      recv_vec4(ptr, reduce4(ٻٽو_انگ), 0);
}

فراغ vvp_fun_signal4_sa::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ٻٽو_انگ,
				      بنانشان بنياد, بنانشان vwid, vvp_context_t)
{
      أكد(bits4_.size() == vwid);
      بنانشان wid = ٻٽو_انگ.size();

      بدل (ptr.پورٽ()) {
	  حالة 0: // Normal input
	    إذا (سونپيو_نقاب.size() == 0) {
                  لكل (بنانشان idx = 0 ;  idx < wid ;  idx += 1) {
			إذا (بنياد+idx >= bits4_.size()) اكسر;
			bits4_.سيٽ_ٻٽو_انگ(بنياد+idx, ٻٽو_انگ.value(idx));
		  }
		  needs_init_ = خطأ;
		  ptr.ptr()->send_vec4(bits4_,0);
	    } وإلا {
		  منطقي بدليو = خطأ;
		  أكد(bits4_.size() == سونپيو_نقاب.size());
		  لكل (بنانشان idx = 0 ;  idx < wid ;  idx += 1) {
			إذا (بنياد+idx >= bits4_.size()) اكسر;
			إذا (سونپيو_نقاب.value(بنياد+idx)) استمر;
			bits4_.سيٽ_ٻٽو_انگ(بنياد+idx, ٻٽو_انگ.value(idx));
			بدليو = سچ;
		  }
		  إذا (بدليو) {
			needs_init_ = خطأ;
			ptr.ptr()->send_vec4(bits4_,0);
		  }
	    }
	    اكسر;

	  حالة 1: // Continuous assign value
	    إذا (سونپيو_نقاب.size() == 0)
		  سونپيو_نقاب = vvp_vector2_t(vvp_vector2_t::FILL0, bits4_.size());
	    لكل (بنانشان idx = 0 ;  idx < wid ;  idx += 1) {
		  إذا (بنياد+idx >= bits4_.size())
			اكسر;
		  bits4_.سيٽ_ٻٽو_انگ(بنياد+idx, ٻٽو_انگ.value(idx));
		  سونپيو_نقاب.سيٽ_ٻٽو_انگ(بنياد+idx, 1);
	    }
	    ptr.ptr()->send_vec4(bits4_,0);
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.پورٽ());
	    أكد(0);
	    اكسر;
      }
}

فراغ vvp_fun_signal4_sa::recv_vec8_pv(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&ٻٽو_انگ,
				      بنانشان بنياد, بنانشان vwid)
{
      recv_vec4_pv(ptr, reduce4(ٻٽو_انگ), بنياد, vwid, 0);
}

فراغ vvp_fun_signal_بنياد::deassign()
{
      continuous_سونپيو_سرگرم = خطأ;
      سونپيو_نقاب = vvp_vector2_t();
}

فراغ vvp_fun_signal_بنياد::deassign_pv(بنانشان بنياد, بنانشان wid)
{
      لكل (بنانشان idx = 0 ;  idx < wid ;  idx += 1) {
	    سونپيو_نقاب.سيٽ_ٻٽو_انگ(بنياد+idx, 0);
      }

      إذا (سونپيو_نقاب.is_zero()) {
	    سونپيو_نقاب = vvp_vector2_t();
      }
}

فراغ automatic_signal_بنياد::release(vvp_net_ptr_t,منطقي)
{
      أكد(0);
}

فراغ automatic_signal_بنياد::release_pv(vvp_net_ptr_t,بنانشان,بنانشان,منطقي)
{
      أكد(0);
}

بنانشان automatic_signal_بنياد::filter_سائيز() ثابت
{
      أكد(0);
      أرجع(0);
}
فراغ automatic_signal_بنياد::force_fil_vec4(ثابت vvp_vector4_t&, ثابت vvp_vector2_t&)
{
      أكد(0);
}
فراغ automatic_signal_بنياد::force_fil_vec8(ثابت vvp_vector8_t&, ثابت vvp_vector2_t&)
{
      أكد(0);
}
فراغ automatic_signal_بنياد::force_fil_real(مزدوج, ثابت vvp_vector2_t&)
{
      أكد(0);
}
فراغ automatic_signal_بنياد::get_قدر(بنية t_vpi_قدر*)
{
      أكد(0);
}

ثابت vvp_vector4_t& vvp_fun_signal4_sa::vec4_unfiltered_قدر() ثابت
{
      أرجع bits4_;
}

vvp_fun_signal4_aa::vvp_fun_signal4_aa(بنانشان wid, vvp_bit4_t init)
{
      context_idx_ = vpip_جوڙيو_شي_منزل_context(هذا, vpip_peek_context_scope());
      سائيز_2 = wid;
      init_ = init;
}

vvp_fun_signal4_aa::~vvp_fun_signal4_aa()
{
      أكد(0);
}

فراغ vvp_fun_signal4_aa::alloc_instance(vvp_context_t context)
{
      vvp_سيٽ_context_شي(context, context_idx_, جديد vvp_vector4_t(سائيز_2, init_));
}

فراغ vvp_fun_signal4_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*بٽ = static_cast<vvp_vector4_t*>
            (vvp_get_context_شي(context, context_idx_));

      بٽ->fill_بٽ(init_);
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ vvp_fun_signal4_aa::free_instance(vvp_context_t context)
{
      vvp_vector4_t*بٽ = static_cast<vvp_vector4_t*>
            (vvp_get_context_شي(context, context_idx_));
      ميٽيو بٽ;
}
#جيڪڏهن_پورو

/*
 * Continuous and forced assignments are not permitted on automatic
 * variables. So we only expect to receive on port 0.
 */
فراغ vvp_fun_signal4_aa::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ٻٽو_انگ,
                                   vvp_context_t context)
{
      أكد(ptr.پورٽ() == 0);
      أكد(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_شي(context, context_idx_));

      إذا (!bits4->eeq(ٻٽو_انگ)) {
            *bits4 = ٻٽو_انگ;
            ptr.ptr()->send_vec4(*bits4, context);
      }
}

فراغ vvp_fun_signal4_aa::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ٻٽو_انگ,
				      بنانشان بنياد, بنانشان vwid, vvp_context_t context)
{
      أكد(ptr.پورٽ() == 0);
      أكد(سائيز_2 == vwid);
      أكد(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_شي(context, context_idx_));

      بنانشان wid = ٻٽو_انگ.size();
      لكل (بنانشان idx = 0 ;  idx < wid ;  idx += 1) {
            إذا (بنياد+idx >= bits4->size()) اكسر;
            bits4->سيٽ_ٻٽو_انگ(بنياد+idx, ٻٽو_انگ.value(idx));
      }
      ptr.ptr()->send_vec4(*bits4, context);
}

بنانشان vvp_fun_signal4_aa::قدر_سائيز() ثابت
{
      أرجع سائيز_2;
}

vvp_bit4_t vvp_fun_signal4_aa::value(بنانشان idx) ثابت
{
      ثابت vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_شي(context_idx_));

      أرجع bits4->value(idx);
}

vvp_scalar_t vvp_fun_signal4_aa::scalar_قدر(بنانشان idx) ثابت
{
      ثابت vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_شي(context_idx_));

      أرجع vvp_scalar_t(bits4->value(idx), 6, 6);
}

فراغ vvp_fun_signal4_aa::vec4_قدر(vvp_vector4_t&val) ثابت
{
      ثابت vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_شي(context_idx_));

      val = *bits4;
}

ثابت vvp_vector4_t&vvp_fun_signal4_aa::vec4_unfiltered_قدر() ثابت
{
      ثابت vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_شي(context_idx_));

      أرجع *bits4;
}

فراغ vvp_fun_signal4_aa::عامل ميٽيو(فراغ*)
{
      أكد(0);
}

/*
 * Testing for equality, we want a bitwise test instead of an
 * arithmetic test because we want to treat for example -0 different
 * from +0.
 */
منطقي بٽ_equal(مزدوج a, مزدوج b)
{
      أرجع memcmp(&a, &b, حجم a) == 0;
}

vvp_fun_signal_real_sa::vvp_fun_signal_real_sa()
{
      بٽ_2 = 0.0;
}

مزدوج vvp_fun_signal_real_sa::real_unfiltered_قدر() ثابت
{
      أرجع بٽ_2;
}

فراغ vvp_fun_signal_real_sa::recv_real(vvp_net_ptr_t ptr, مزدوج ٻٽو_انگ,
                                       vvp_context_t)
{
      بدل (ptr.پورٽ()) {
	  حالة 0:
	    إذا (!continuous_سونپيو_سرگرم) {
                  إذا (needs_init_ || !بٽ_equal(بٽ_2, ٻٽو_انگ)) {
			بٽ_2 = ٻٽو_انگ;
			needs_init_ = خطأ;
			ptr.ptr()->send_real(ٻٽو_انگ, 0);
		  }
	    }
	    اكسر;

	  حالة 1: // Continuous assign value
	    continuous_سونپيو_سرگرم = سچ;
	    بٽ_2 = ٻٽو_انگ;
	    ptr.ptr()->send_real(ٻٽو_انگ, 0);
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.پورٽ());
	    أكد(0);
	    اكسر;
      }
}

vvp_fun_signal_real_aa::vvp_fun_signal_real_aa()
{
      context_idx_ = vpip_جوڙيو_شي_منزل_context(هذا, vpip_peek_context_scope());
}

vvp_fun_signal_real_aa::~vvp_fun_signal_real_aa()
{
      أكد(0);
}

فراغ vvp_fun_signal_real_aa::alloc_instance(vvp_context_t context)
{
      مزدوج*بٽ = جديد مزدوج;
      vvp_سيٽ_context_شي(context, context_idx_, بٽ);

      *بٽ = 0.0;
}

فراغ vvp_fun_signal_real_aa::reset_instance(vvp_context_t context)
{
      مزدوج*بٽ = static_cast<مزدوج*>
            (vvp_get_context_شي(context, context_idx_));

      *بٽ = 0.0;
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ vvp_fun_signal_real_aa::free_instance(vvp_context_t context)
{
      مزدوج*بٽ = static_cast<مزدوج*>
            (vvp_get_context_شي(context, context_idx_));
      ميٽيو بٽ;
}
#جيڪڏهن_پورو

مزدوج vvp_fun_signal_real_aa::real_unfiltered_قدر() ثابت
{
      ثابت مزدوج*بٽ = static_cast<مزدوج*>
            (vthread_get_rd_context_شي(context_idx_));

      أرجع *بٽ;
}

مزدوج vvp_fun_signal_real_aa::real_قدر() ثابت
{
      أرجع real_unfiltered_قدر();
}

فراغ vvp_fun_signal_real_aa::recv_real(vvp_net_ptr_t ptr, مزدوج ٻٽو_انگ,
                                       vvp_context_t context)
{
      أكد(ptr.پورٽ() == 0);
      أكد(context);

      مزدوج*بٽ = static_cast<مزدوج*>
            (vvp_get_context_شي(context, context_idx_));

      إذا (!بٽ_equal(*بٽ,ٻٽو_انگ)) {
            *بٽ = ٻٽو_انگ;
            ptr.ptr()->send_real(ٻٽو_انگ, context);
      }
}

بنانشان vvp_fun_signal_real_aa::قدر_سائيز() ثابت
{
      أكد(0);
      أرجع 1;
}

vvp_bit4_t vvp_fun_signal_real_aa::value(بنانشان) ثابت
{
      أكد(0);
      أرجع BIT4_X;
}

vvp_scalar_t vvp_fun_signal_real_aa::scalar_قدر(بنانشان) ثابت
{
      أكد(0);
      أرجع vvp_scalar_t();
}

فراغ vvp_fun_signal_real_aa::vec4_قدر(vvp_vector4_t&) ثابت
{
      أكد(0);
}

فراغ* vvp_fun_signal_real_aa::عامل جديد(std::size_t size)
{
      أرجع vvp_net_fun_t::ڍير.alloc(size);
}

فراغ vvp_fun_signal_real_aa::عامل ميٽيو(فراغ*)
{
      أكد(0);
}


vvp_fun_signal_string_sa::vvp_fun_signal_string_sa()
{
}

فراغ vvp_fun_signal_string_sa::recv_string(vvp_net_ptr_t ptr, ثابت std::string&ٻٽو_انگ,
					   vvp_context_t)
{
      أكد(ptr.پورٽ() == 0);

      إذا (needs_init_ || قدر_2 != ٻٽو_انگ) {
	    قدر_2 = ٻٽو_انگ;
	    needs_init_ = خطأ;

	    ptr.ptr()->send_string(ٻٽو_انگ, 0);
      }
}

ثابت string& vvp_fun_signal_string_sa::get_string() ثابت
{
      أرجع قدر_2;
}

vvp_fun_signal_string_aa::vvp_fun_signal_string_aa()
{
      context_idx_ = vpip_جوڙيو_شي_منزل_context(هذا, vpip_peek_context_scope());
}

vvp_fun_signal_string_aa::~vvp_fun_signal_string_aa()
{
      أكد(0);
}

فراغ vvp_fun_signal_string_aa::alloc_instance(vvp_context_t context)
{
      string*بٽ = جديد std::string;
      vvp_سيٽ_context_شي(context, context_idx_, بٽ);
      *بٽ = "";
}

فراغ vvp_fun_signal_string_aa::reset_instance(vvp_context_t context)
{
      string*بٽ = static_cast<std::string*>
	    (vvp_get_context_شي(context, context_idx_));
      *بٽ = "";
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ vvp_fun_signal_string_aa::free_instance(vvp_context_t context)
{
      string*بٽ = static_cast<std::string*>
            (vvp_get_context_شي(context, context_idx_));
      ميٽيو بٽ;
}
#جيڪڏهن_پورو

فراغ vvp_fun_signal_string_aa::recv_string(vvp_net_ptr_t ptr, ثابت std::string&ٻٽو_انگ, vvp_context_t context)
{
      أكد(ptr.پورٽ() == 0);
      أكد(context);

      string*بٽ = static_cast<std::string*>
	    (vvp_get_context_شي(context, context_idx_));

      إذا (*بٽ != ٻٽو_انگ) {
	    *بٽ = ٻٽو_انگ;
	    ptr.ptr()->send_string(ٻٽو_انگ, context);
      }
}

بنانشان vvp_fun_signal_string_aa::قدر_سائيز() ثابت
{
      أكد(0);
      أرجع 1;
}

vvp_bit4_t vvp_fun_signal_string_aa::value(بنانشان) ثابت
{
      أكد(0);
      أرجع BIT4_X;
}

vvp_scalar_t vvp_fun_signal_string_aa::scalar_قدر(بنانشان) ثابت
{
      أكد(0);
      أرجع vvp_scalar_t();
}

فراغ vvp_fun_signal_string_aa::vec4_قدر(vvp_vector4_t&) ثابت
{
      أكد(0);
}

مزدوج vvp_fun_signal_string_aa::real_قدر() ثابت
{
      أكد(0);
      أرجع 0.0;
}

ثابت std::string& vvp_fun_signal_string_aa::get_string() ثابت
{
      ثابت string*بٽ = static_cast<std::string*>
            (vthread_get_rd_context_شي(context_idx_));

      أرجع *بٽ;
}

فراغ* vvp_fun_signal_string_aa::عامل جديد(std::size_t size)
{
      أرجع vvp_net_fun_t::ڍير.alloc(size);
}

فراغ vvp_fun_signal_string_aa::عامل ميٽيو(فراغ*)
{
      أكد(0);
}

  /* OBJECT signals */

vvp_fun_signal_object_sa::vvp_fun_signal_object_sa(بنانشان size)
: vvp_fun_signal_object(size)
{
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ vvp_fun_signal_object_aa::free_instance(vvp_context_t context)
{
      vvp_object_t*بٽ = static_cast<vvp_object_t*>
            (vvp_get_context_شي(context, context_idx_));
      ميٽيو بٽ;
}
#جيڪڏهن_پورو

فراغ vvp_fun_signal_object_sa::recv_object(vvp_net_ptr_t ptr, vvp_object_t ٻٽو_انگ,
					   vvp_context_t)
{
      أكد(ptr.پورٽ() == 0);

      إذا (needs_init_ || قدر_2 != ٻٽو_انگ) {
	    قدر_2 = ٻٽو_انگ;
	    needs_init_ = خطأ;

	    ptr.ptr()->send_object(ٻٽو_انگ, 0);
      }
}

vvp_object_t vvp_fun_signal_object_sa::get_object() ثابت
{
      أرجع قدر_2;
}

vvp_fun_signal_object_aa::vvp_fun_signal_object_aa(بنانشان size)
: vvp_fun_signal_object(size)
{
      context_idx_ = vpip_جوڙيو_شي_منزل_context(هذا, vpip_peek_context_scope());
}

vvp_fun_signal_object_aa::~vvp_fun_signal_object_aa()
{
      أكد(0);
}

فراغ vvp_fun_signal_object_aa::alloc_instance(vvp_context_t context)
{
      vvp_object_t*بٽ = جديد vvp_object_t;
      vvp_سيٽ_context_شي(context, context_idx_, بٽ);
      بٽ->reset();
}

فراغ vvp_fun_signal_object_aa::reset_instance(vvp_context_t context)
{
      vvp_object_t*بٽ = static_cast<vvp_object_t*>
	    (vvp_get_context_شي(context, context_idx_));
      بٽ->reset();
}

vvp_object_t vvp_fun_signal_object_aa::get_object() ثابت
{
      ثابت vvp_object_t*بٽ = static_cast<vvp_object_t*>
	    (vthread_get_rd_context_شي(context_idx_));
      أرجع *بٽ;
}

فراغ vvp_fun_signal_object_aa::recv_object(vvp_net_ptr_t ptr, vvp_object_t ٻٽو_انگ,
					   vvp_context_t context)
{
      أكد(ptr.پورٽ() == 0);
      أكد(context);

      vvp_object_t*بٽ = static_cast<vvp_object_t*>
	    (vvp_get_context_شي(context, context_idx_));

      إذا (*بٽ != ٻٽو_انگ) {
	    *بٽ = ٻٽو_انگ;
	    ptr.ptr()->send_object(ٻٽو_انگ, context);
      }
}

بنانشان vvp_fun_signal_object_aa::قدر_سائيز() ثابت
{
      أرجع 1;
}

vvp_bit4_t vvp_fun_signal_object_aa::value(بنانشان) ثابت
{
      أكد(0);
      أرجع BIT4_X;
}

vvp_scalar_t vvp_fun_signal_object_aa::scalar_قدر(بنانشان) ثابت
{
      أكد(0);
      أرجع vvp_scalar_t();
}

فراغ vvp_fun_signal_object_aa::vec4_قدر(vvp_vector4_t&) ثابت
{
      أكد(0);
}

فراغ* vvp_fun_signal_object_aa::عامل جديد(std::size_t size)
{
      أرجع vvp_net_fun_t::ڍير.alloc(size);
}

فراغ vvp_fun_signal_object_aa::عامل ميٽيو(فراغ*)
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

فراغ vvp_fun_force::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ٻٽو_انگ,
			      vvp_context_t)
{
      أكد(ptr.پورٽ() == 0);
      vvp_net_t*net = ptr.ptr();

      vvp_net_t*dst = net->پورٽ[3].ptr();
      أكد(dst->fil);

      dst->force_vec4(coerce_منزل_ويڪر(ٻٽو_انگ, dst->fil->filter_سائيز()), vvp_vector2_t(vvp_vector2_t::FILL1, dst->fil->filter_سائيز()));
}

فراغ vvp_fun_force::recv_real(vvp_net_ptr_t ptr, مزدوج ٻٽو_انگ, vvp_context_t)
{
      أكد(ptr.پورٽ() == 0);
      vvp_net_t*net = ptr.ptr();
      vvp_net_t*dst = net->پورٽ[3].ptr();
      dst->force_real(ٻٽو_انگ, vvp_vector2_t(vvp_vector2_t::FILL1, 1));
}

vvp_wire_بنياد::vvp_wire_بنياد()
{
}

vvp_wire_بنياد::~vvp_wire_بنياد()
{
}

vvp_bit4_t vvp_wire_بنياد::driven_قدر(بنانشان) ثابت
{
      أكد(0);
      أرجع BIT4_X;
}

منطقي vvp_wire_بنياد::is_forced(بنانشان) ثابت
{
      أكد(0);
      أرجع خطأ;
}

vvp_wire_vec4::vvp_wire_vec4(بنانشان wid, vvp_bit4_t init)
: bits4_(wid, init)
{
      needs_init_ = سچ;
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec4(ثابت vvp_vector4_t&ٻٽو_انگ, vvp_vector4_t&rep,
						 بنانشان بنياد, بنانشان vwid)
{
	// Special case! the input bit is 0 wid. Interpret this as a
	// vector of BIT4_X to match the width of the bits4_ vector.
	// FIXME! This is a hack to work around some buggy gate
	// implementations! This should be removed!
      إذا (بنياد==0 && vwid==0) {
	    vvp_vector4_t tmp (bits4_.size(), BIT4_X);
	    إذا (bits4_ .eeq(tmp) && !needs_init_) أرجع STOP;
	    bits4_ = tmp;
	    needs_init_ = خطأ;
	    أرجع filter_نقاب(tmp, force4_, rep, 0);
      }

      إذا (vwid != bits4_.size()) {
	    cerr << "Internal error: Input vector expected width="
		 << bits4_.size() << ", got "
		 << "bit=" << ٻٽو_انگ << ", base=" << بنياد << ", vwid=" << vwid
		 << سٽ_پڄاڻي;
      }
      أكد(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      إذا (بنياد==0 && ٻٽو_انگ.size()==vwid) {
	    إذا (bits4_ .eeq( ٻٽو_انگ ) && !needs_init_) أرجع STOP;
	    bits4_ = ٻٽو_انگ;
      } وإلا {
	    منطقي rc = bits4_.سيٽ_vec(بنياد, ٻٽو_انگ);
	    إذا (rc == خطأ && !needs_init_) أرجع STOP;
      }

      needs_init_ = خطأ;
      أرجع filter_نقاب(ٻٽو_انگ, force4_, rep, بنياد);
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec8(ثابت vvp_vector8_t&ٻٽو_انگ,
                                                 vvp_vector8_t&rep,
                                                 بنانشان بنياد,
                                                 بنانشان vwid)
{
      أكد(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      vvp_vector4_t bit4 (reduce4(ٻٽو_انگ));
      إذا (بنياد==0 && bit4.size()==vwid) {
	    إذا (bits4_ .eeq( bit4 ) && !needs_init_) أرجع STOP;
	    bits4_ = bit4;
      } وإلا {
	    منطقي rc = bits4_.سيٽ_vec(بنياد, bit4);
	    إذا (rc == خطأ && !needs_init_) أرجع STOP;
      }

      needs_init_ = خطأ;
      أرجع filter_نقاب(ٻٽو_انگ, vvp_vector8_t(force4_,6,6), rep, بنياد);
}

بنانشان vvp_wire_vec4::filter_سائيز() ثابت
{
      أرجع bits4_.size();
}

فراغ vvp_wire_vec4::force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&نقاب)
{
      force_نقاب(نقاب);

      إذا (force4_.size() == 0) {
	    force4_ = val;
      } وإلا {
	    لكل (بنانشان idx = 0; idx < نقاب.size() ; idx += 1) {
		  إذا (نقاب.value(idx) == 0)
			استمر;

		  force4_.سيٽ_ٻٽو_انگ(idx, val.value(idx));
	    }
      }
      لاڳيتو_vpi_callbacks();
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
      vvp_vector2_t نقاب (vvp_vector2_t::FILL1, bits4_.size());
      إذا (net_flag) {
	      // Wires revert to their unforced value after release.
            release_نقاب(نقاب);
	    needs_init_ = ! force4_ .eeq(bits4_);
	    ptr.ptr()->send_vec4(bits4_, 0);
	    لاڳيتو_vpi_callbacks();
      } وإلا {
	      // Variables keep the current value.
	    vvp_vector4_t res (bits4_.size());
	    لكل (بنانشان idx=0; idx<bits4_.size(); idx += 1)
		  res.سيٽ_ٻٽو_انگ(idx,value(idx));
            release_نقاب(نقاب);
	    ptr.ptr()->fun->recv_vec4(ptr, res, 0);
      }
}

فراغ vvp_wire_vec4::release_pv(vvp_net_ptr_t ptr, بنانشان بنياد, بنانشان wid, منطقي net_flag)
{
      أكد(bits4_.size() >= بنياد + wid);

      vvp_vector2_t نقاب (vvp_vector2_t::FILL0, bits4_.size());
      لكل (بنانشان idx = 0 ; idx < wid ; idx += 1)
	    نقاب.سيٽ_ٻٽو_انگ(بنياد+idx, 1);

      إذا (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_نقاب(نقاب);
	    needs_init_ = ! force4_.subvalue(بنياد,wid) .eeq(bits4_.subvalue(بنياد,wid));
	    ptr.ptr()->send_vec4_pv(bits4_.subvalue(بنياد,wid),
				    بنياد, bits4_.size(), 0);
	    لاڳيتو_vpi_callbacks();
      } وإلا {
	      // Variables keep the current value.
	    vvp_vector4_t res (wid);
	    لكل (بنانشان idx=0; idx<wid; idx += 1)
		  res.سيٽ_ٻٽو_انگ(idx,value(بنياد+idx));
	    release_نقاب(نقاب);
	    ptr.ptr()->fun->recv_vec4_pv(ptr, res, بنياد, bits4_.size(), 0);
      }
}

بنانشان vvp_wire_vec4::قدر_سائيز() ثابت
{
      أرجع bits4_.size();
}

vvp_bit4_t vvp_wire_vec4::filtered_قدر(بنانشان idx) ثابت
{
      إذا (test_force_نقاب(idx))
	    أرجع force4_.value(idx);
      وإلا
	    أرجع bits4_.value(idx);
}

vvp_bit4_t vvp_wire_vec4::value(بنانشان idx) ثابت
{
      أرجع filtered_قدر(idx);
}

vvp_scalar_t vvp_wire_vec4::scalar_قدر(بنانشان idx) ثابت
{
      أرجع vvp_scalar_t(value(idx),6,6);
}

فراغ vvp_wire_vec4::vec4_قدر(vvp_vector4_t&val) ثابت
{
      val = bits4_;
      إذا (test_force_نقاب_is_zero())
	    أرجع;

      لكل (بنانشان idx = 0 ; idx < bits4_.size() ; idx += 1)
	    val.سيٽ_ٻٽو_انگ(idx, filtered_قدر(idx));
}

vvp_bit4_t vvp_wire_vec4::driven_قدر(بنانشان idx) ثابت
{
      أرجع bits4_.value(idx);
}

منطقي vvp_wire_vec4::is_forced(بنانشان idx) ثابت
{
      أرجع test_force_نقاب(idx);
}

vvp_wire_vec8::vvp_wire_vec8(بنانشان wid)
: bits8_(wid)
{
      needs_init_ = سچ;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec4(ثابت vvp_vector4_t&ٻٽو_انگ,
                                                 vvp_vector4_t&rep,
                                                 بنانشان بنياد,
                                                 بنانشان vwid)
{
	// For now there is no support for a non-zero base.
      أكد(0 == بنياد);
      أكد(bits8_.size() == vwid);
      أكد(bits8_.size() == ٻٽو_انگ.size());
	// QUESTION: Is it really correct to propagate a vec4 if this
	// is a vec8 node? In fact, it is really possible for a vec4
	// value to get through to a vec8 filter?
      vvp_vector8_t rep8;
      prop_t rc = filter_vec8(vvp_vector8_t(ٻٽو_انگ,6,6), rep8, 0, vwid);
      إذا (rc == REPL)
	    rep = reduce4(rep8);

      needs_init_ = خطأ;
      أرجع rc;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec8(ثابت vvp_vector8_t&ٻٽو_انگ, vvp_vector8_t&rep, بنانشان بنياد, بنانشان vwid)
{
      أكد(vwid == bits8_.size());
	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      إذا (بنياد==0 && ٻٽو_انگ.size()==vwid) {
	    إذا (bits8_.eeq(ٻٽو_انگ) && !needs_init_) أرجع STOP;
	    bits8_ = ٻٽو_انگ;
      } وإلا {
	    إذا (bits8_.subvalue(بنياد, ٻٽو_انگ.size()).eeq(ٻٽو_انگ) &&
		!needs_init_) {
		  أرجع STOP;
	    }
	    إذا (bits8_.size() == 0)
		  bits8_ = vvp_vector8_t(vwid);
	    أكد(bits8_.size() == vwid);
	    bits8_.سيٽ_vec(بنياد, ٻٽو_انگ);
      }
      needs_init_ = خطأ;
      أرجع filter_نقاب(ٻٽو_انگ, force8_, rep, بنياد);
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_داخلا_vec8(ثابت vvp_vector8_t&ٻٽو_انگ, vvp_vector8_t&rep) ثابت
{
      أرجع filter_داخلا_نقاب(ٻٽو_انگ, force8_, rep);
}

بنانشان vvp_wire_vec8::filter_سائيز() ثابت
{
      أرجع bits8_.size();
}

فراغ vvp_wire_vec8::force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&نقاب)
{
      force_fil_vec8(vvp_vector8_t(val,6,6), نقاب);
}

فراغ vvp_wire_vec8::force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&نقاب)
{
      force_نقاب(نقاب);

      إذا (force8_.size() == 0) {
	    force8_ = val;
      } وإلا {
	    لكل (بنانشان idx = 0; idx < نقاب.size() ; idx += 1) {
		  إذا (نقاب.value(idx) == 0)
			استمر;

		  force8_.سيٽ_ٻٽو_انگ(idx, val.value(idx));
	    }
      }
      لاڳيتو_vpi_callbacks();
}

فراغ vvp_wire_vec8::force_fil_real(مزدوج, ثابت vvp_vector2_t&)
{
      أكد(0);
}

فراغ vvp_wire_vec8::release(vvp_net_ptr_t ptr, منطقي net_flag)
{
	// Wires revert to their unforced value after release.
      vvp_vector2_t نقاب (vvp_vector2_t::FILL1, bits8_.size());
      release_نقاب(نقاب);
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

فراغ vvp_wire_vec8::release_pv(vvp_net_ptr_t ptr, بنانشان بنياد, بنانشان wid, منطقي net_flag)
{
      أكد(bits8_.size() >= بنياد + wid);

      vvp_vector2_t نقاب (vvp_vector2_t::FILL0, bits8_.size());
      لكل (بنانشان idx = 0 ; idx < wid ; idx += 1)
	    نقاب.سيٽ_ٻٽو_انگ(بنياد+idx, 1);

      release_نقاب(نقاب);

      إذا (net_flag) {
	    needs_init_ = !force8_.subvalue(بنياد,wid) .eeq((bits8_.subvalue(بنياد,wid)));
	    ptr.ptr()->send_vec8_pv(bits8_.subvalue(بنياد,wid),
				    بنياد, bits8_.size());
	    لاڳيتو_vpi_callbacks();
      } وإلا {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    أكد(0);
//	    ptr.ptr()->fun->recv_vec8_pv(ptr, force8_.subvalue(base,wid),
//					 base, force8_.size());
      }
}

بنانشان vvp_wire_vec8::قدر_سائيز() ثابت
{
      أرجع bits8_.size();
}

vvp_scalar_t vvp_wire_vec8::filtered_قدر(بنانشان idx) ثابت
{
      إذا (test_force_نقاب(idx))
	    أرجع force8_.value(idx);
      وإلا
	    أرجع bits8_.value(idx);
}

vvp_bit4_t vvp_wire_vec8::value(بنانشان idx) ثابت
{
      أرجع filtered_قدر(idx).value();
}

vvp_scalar_t vvp_wire_vec8::scalar_قدر(بنانشان idx) ثابت
{
      أرجع filtered_قدر(idx);
}

vvp_vector8_t vvp_wire_vec8::vec8_قدر() ثابت
{
      vvp_vector8_t tmp = bits8_;
      لكل (بنانشان idx = 0 ; idx < bits8_.size() ; idx += 1)
	    tmp.سيٽ_ٻٽو_انگ(idx, filtered_قدر(idx));
      أرجع tmp;
}

فراغ vvp_wire_vec8::vec4_قدر(vvp_vector4_t&val) ثابت
{
      val = reduce4(vec8_قدر());
}

vvp_bit4_t vvp_wire_vec8::driven_قدر(بنانشان idx) ثابت
{
      أرجع bits8_.value(idx).value();
}

منطقي vvp_wire_vec8::is_forced(بنانشان idx) ثابت
{
      أرجع test_force_نقاب(idx);
}

vvp_wire_real::vvp_wire_real()
: ٻٽو_انگ_2(0.0), force_(0.0)
{
}

vvp_net_fil_t::prop_t vvp_wire_real::filter_real(مزدوج&ٻٽو_انگ)
{
      إذا (بٽ_equal(ٻٽو_انگ_2, ٻٽو_انگ) && !needs_init_) أرجع STOP;
      ٻٽو_انگ_2 = ٻٽو_انگ;
      needs_init_ = خطأ;
      أرجع filter_نقاب(ٻٽو_انگ, force_);
}

بنانشان vvp_wire_real::filter_سائيز() ثابت
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

فراغ vvp_wire_real::force_fil_real(مزدوج val, ثابت vvp_vector2_t&نقاب)
{
      force_نقاب(نقاب);
      إذا (نقاب.value(0))
	    force_ = val;

      لاڳيتو_vpi_callbacks();
}

فراغ vvp_wire_real::release(vvp_net_ptr_t ptr, منطقي net_flag)
{
      vvp_vector2_t نقاب (vvp_vector2_t::FILL1, 1);
      إذا (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_نقاب(نقاب);
	    needs_init_ = !بٽ_equal(force_, ٻٽو_انگ_2);
	    ptr.ptr()->send_real(ٻٽو_انگ_2, 0);
      } وإلا {
	      // Variables keep the current value.
	    مزدوج res =  real_قدر();
	    release_نقاب(نقاب);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
}

فراغ vvp_wire_real::release_pv(vvp_net_ptr_t, بنانشان, بنانشان, منطقي)
{
      أكد(0);
#إذا 0
	// A real is a single value. If for some reason this part release
	// can happen the following code should work correctly (requires
	// a base of 0 and a width of 1).
      vvp_vector2_t نقاب (vvp_vector2_t::FILL1, 1);
      أكد(بنياد == 0 && wid == 1);

      إذا (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_نقاب(نقاب);
	    ptr.ptr()->send_real(ٻٽو_انگ_2, 0);
      } وإلا {
	      // Variables keep the current value.
	    مزدوج res =  real_قدر();
	    release_نقاب(نقاب);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
#جيڪڏهن_پورو
}

بنانشان vvp_wire_real::قدر_سائيز() ثابت
{
      أكد(0);
      أرجع 1;
}

vvp_bit4_t vvp_wire_real::value(بنانشان) ثابت
{
      أكد(0);
      أرجع BIT4_X;
}

vvp_scalar_t vvp_wire_real::scalar_قدر(بنانشان) ثابت
{
      أكد(0);
      أرجع vvp_scalar_t();
}

فراغ vvp_wire_real::vec4_قدر(vvp_vector4_t&) ثابت
{
      أكد(0);
}

مزدوج vvp_wire_real::real_قدر() ثابت
{
      إذا (test_force_نقاب(0))
	    أرجع force_;
      وإلا
	    أرجع ٻٽو_انگ_2;
}

#إذا 0
vvp_wire_string::vvp_wire_string()
{
}

بنانشان vvp_wire_string::filter_سائيز() ثابت
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

فراغ vvp_wire_string::release_pv(vvp_net_ptr_t, بنانشان, بنانشان, منطقي)
{
      أكد(0);
}

بنانشان vvp_wire_string::قدر_سائيز() ثابت
{
      أكد(0);
      أرجع 1;
}

vvp_bit4_t vvp_wire_string::value(بنانشان) ثابت
{
      أكد(0);
      أرجع BIT4_X;
}

vvp_scalar_t vvp_wire_string::scalar_قدر(بنانشان) ثابت
{
      أكد(0);
      أرجع vvp_scalar_t();
}

فراغ vvp_wire_string::vec4_قدر(vvp_vector4_t&) ثابت
{
      أكد(0);
}

مزدوج vvp_wire_string::real_قدر() ثابت
{
      أكد(0);
      أرجع 0.0;
}
#جيڪڏهن_پورو
