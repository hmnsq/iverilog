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
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
# include  <map>
#جے_انت

# include  <iostream>

ورتوں نطاق std;

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
سانچا <صنف T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_نقاب(ثابت T&val, ثابت T&force, T&filter, بنانشانی ادھار)
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
	    أكد((ادھار+val.size()) <= force_نقاب_2.size());

	    filter = val;
	    لكل (بنانشانی idx = 0 ; idx < val.size() ; idx += 1) {
		  إذا (force_نقاب_2.value(ادھار+idx))
			filter.سیٹ_ثنائی_ہندسہ(idx, force.value(ادھار+idx));
		  وإلا
			propagate_flag = سچ;
	    }

	    إذا (propagate_flag) {
		  لگاتار_vpi_callbacks();
		  أرجع REPL;
	    } وإلا {
		  أرجع STOP;
	    }

      } وإلا {
	    لگاتار_vpi_callbacks();
	    أرجع PROP;
      }
}

سانچا <صنف T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_نقاب(T&val, T force)
{

      إذا (test_force_نقاب(0)) {
	    val = force;
	    لگاتار_vpi_callbacks();
	    أرجع REPL;
      }
      لگاتار_vpi_callbacks();
      أرجع PROP;
}

سانچا <صنف T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_داخلہ_نقاب(ثابت T&val, ثابت T&force, T&rep) ثابت
{
      إذا (test_force_نقاب_is_zero())
	    أرجع PROP;

      أكد(force_نقاب_2.size() == force.size());

      rep = val;
      لكل (بنانشانی idx = 0 ; idx < val.size() ; idx += 1) {
	    إذا (force_نقاب_2.value(idx))
		  rep.سیٹ_ثنائی_ہندسہ(idx, force.value(idx));
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
      vvp_send_vec4(باہرلا_2, val, 0);
}

فراغ vvp_net_t::force_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&نقاب)
{
      أكد(fil);
      fil->force_fil_vec8(val, نقاب);
      fun->force_flag(خطأ);
      vvp_send_vec8(باہرلا_2, val);
}

فراغ vvp_net_t::force_real(مزدوج val, ثابت vvp_vector2_t&نقاب)
{
      أكد(fil);
      fil->force_fil_real(val, نقاب);
      fun->force_flag(خطأ);
      vvp_send_real(باہرلا_2, val, 0);
}

/* **** vvp_fun_signal methods **** */

vvp_fun_signal_ادھار::vvp_fun_signal_ادھار()
{
      continuous_سونپو_سرگرم = خطأ;
      needs_init_ = سچ;
      cassign_link = 0;
      گنتی_functors_sig += 1;
}

vvp_fun_signal4_sa::vvp_fun_signal4_sa(بنانشانی wid, vvp_bit4_t init)
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
فراغ vvp_fun_signal4_sa::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                   vvp_context_t)
{
      بدل (ptr.پورٹ()) {
	  حالة 0: // Normal input (feed from net, or set from process)
	      /* If we don't have a continuous assign mask then just
		 copy the bits, otherwise we need to see if there are
		 any holes in the mask so we can set those bits. */
	    إذا (سونپو_نقاب.size() == 0) {
                  إذا (needs_init_ || !bits4_.eeq(ثنائی_ہندسہ)) {
			أكد(ثنائی_ہندسہ.size() == bits4_.size());
			bits4_ = ثنائی_ہندسہ;
			needs_init_ = خطأ;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    } وإلا {
		  منطقي بدلیا = خطأ;
		  أكد(bits4_.size() == سونپو_نقاب.size());
		  لكل (بنانشانی idx = 0 ;  idx < ثنائی_ہندسہ.size() ;  idx += 1) {
			إذا (idx >= bits4_.size()) اكسر;
			إذا (سونپو_نقاب.value(idx)) استمر;
			bits4_.سیٹ_ثنائی_ہندسہ(idx, ثنائی_ہندسہ.value(idx));
			بدلیا = سچ;
		  }
		  إذا (بدلیا) {
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
	    bits4_ = coerce_منزل_چوڑائی(ثنائی_ہندسہ, bits4_.size());
	    سونپو_نقاب = vvp_vector2_t(vvp_vector2_t::FILL1, bits4_.size());
	    ptr.ptr()->send_vec4(bits4_, 0);
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.پورٹ());
	    أكد(0);
	    اكسر;
      }
}

فراغ vvp_fun_signal4_sa::recv_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&ثنائی_ہندسہ)
{
      recv_vec4(ptr, reduce4(ثنائی_ہندسہ), 0);
}

فراغ vvp_fun_signal4_sa::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				      بنانشانی ادھار, بنانشانی vwid, vvp_context_t)
{
      أكد(bits4_.size() == vwid);
      بنانشانی wid = ثنائی_ہندسہ.size();

      بدل (ptr.پورٹ()) {
	  حالة 0: // Normal input
	    إذا (سونپو_نقاب.size() == 0) {
                  لكل (بنانشانی idx = 0 ;  idx < wid ;  idx += 1) {
			إذا (ادھار+idx >= bits4_.size()) اكسر;
			bits4_.سیٹ_ثنائی_ہندسہ(ادھار+idx, ثنائی_ہندسہ.value(idx));
		  }
		  needs_init_ = خطأ;
		  ptr.ptr()->send_vec4(bits4_,0);
	    } وإلا {
		  منطقي بدلیا = خطأ;
		  أكد(bits4_.size() == سونپو_نقاب.size());
		  لكل (بنانشانی idx = 0 ;  idx < wid ;  idx += 1) {
			إذا (ادھار+idx >= bits4_.size()) اكسر;
			إذا (سونپو_نقاب.value(ادھار+idx)) استمر;
			bits4_.سیٹ_ثنائی_ہندسہ(ادھار+idx, ثنائی_ہندسہ.value(idx));
			بدلیا = سچ;
		  }
		  إذا (بدلیا) {
			needs_init_ = خطأ;
			ptr.ptr()->send_vec4(bits4_,0);
		  }
	    }
	    اكسر;

	  حالة 1: // Continuous assign value
	    إذا (سونپو_نقاب.size() == 0)
		  سونپو_نقاب = vvp_vector2_t(vvp_vector2_t::FILL0, bits4_.size());
	    لكل (بنانشانی idx = 0 ;  idx < wid ;  idx += 1) {
		  إذا (ادھار+idx >= bits4_.size())
			اكسر;
		  bits4_.سیٹ_ثنائی_ہندسہ(ادھار+idx, ثنائی_ہندسہ.value(idx));
		  سونپو_نقاب.سیٹ_ثنائی_ہندسہ(ادھار+idx, 1);
	    }
	    ptr.ptr()->send_vec4(bits4_,0);
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.پورٹ());
	    أكد(0);
	    اكسر;
      }
}

فراغ vvp_fun_signal4_sa::recv_vec8_pv(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&ثنائی_ہندسہ,
				      بنانشانی ادھار, بنانشانی vwid)
{
      recv_vec4_pv(ptr, reduce4(ثنائی_ہندسہ), ادھار, vwid, 0);
}

فراغ vvp_fun_signal_ادھار::deassign()
{
      continuous_سونپو_سرگرم = خطأ;
      سونپو_نقاب = vvp_vector2_t();
}

فراغ vvp_fun_signal_ادھار::deassign_pv(بنانشانی ادھار, بنانشانی wid)
{
      لكل (بنانشانی idx = 0 ;  idx < wid ;  idx += 1) {
	    سونپو_نقاب.سیٹ_ثنائی_ہندسہ(ادھار+idx, 0);
      }

      إذا (سونپو_نقاب.is_zero()) {
	    سونپو_نقاب = vvp_vector2_t();
      }
}

فراغ automatic_signal_ادھار::release(vvp_net_ptr_t,منطقي)
{
      أكد(0);
}

فراغ automatic_signal_ادھار::release_pv(vvp_net_ptr_t,بنانشانی,بنانشانی,منطقي)
{
      أكد(0);
}

بنانشانی automatic_signal_ادھار::filter_ناپ() ثابت
{
      أكد(0);
      أرجع(0);
}
فراغ automatic_signal_ادھار::force_fil_vec4(ثابت vvp_vector4_t&, ثابت vvp_vector2_t&)
{
      أكد(0);
}
فراغ automatic_signal_ادھار::force_fil_vec8(ثابت vvp_vector8_t&, ثابت vvp_vector2_t&)
{
      أكد(0);
}
فراغ automatic_signal_ادھار::force_fil_real(مزدوج, ثابت vvp_vector2_t&)
{
      أكد(0);
}
فراغ automatic_signal_ادھار::get_قدر(بنية t_vpi_قدر*)
{
      أكد(0);
}

ثابت vvp_vector4_t& vvp_fun_signal4_sa::vec4_unfiltered_قدر() ثابت
{
      أرجع bits4_;
}

vvp_fun_signal4_aa::vvp_fun_signal4_aa(بنانشانی wid, vvp_bit4_t init)
{
      context_idx_ = vpip_جوڑو_شے_منزل_context(هذا, vpip_peek_context_scope());
      ناپ_2 = wid;
      init_ = init;
}

vvp_fun_signal4_aa::~vvp_fun_signal4_aa()
{
      أكد(0);
}

فراغ vvp_fun_signal4_aa::alloc_instance(vvp_context_t context)
{
      vvp_سیٹ_context_شے(context, context_idx_, جديد vvp_vector4_t(ناپ_2, init_));
}

فراغ vvp_fun_signal4_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*بٹ = static_cast<vvp_vector4_t*>
            (vvp_get_context_شے(context, context_idx_));

      بٹ->fill_بٹ(init_);
}

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
فراغ vvp_fun_signal4_aa::free_instance(vvp_context_t context)
{
      vvp_vector4_t*بٹ = static_cast<vvp_vector4_t*>
            (vvp_get_context_شے(context, context_idx_));
      مٹاؤ بٹ;
}
#جے_انت

/*
 * Continuous and forced assignments are not permitted on automatic
 * variables. So we only expect to receive on port 0.
 */
فراغ vvp_fun_signal4_aa::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                   vvp_context_t context)
{
      أكد(ptr.پورٹ() == 0);
      أكد(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_شے(context, context_idx_));

      إذا (!bits4->eeq(ثنائی_ہندسہ)) {
            *bits4 = ثنائی_ہندسہ;
            ptr.ptr()->send_vec4(*bits4, context);
      }
}

فراغ vvp_fun_signal4_aa::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				      بنانشانی ادھار, بنانشانی vwid, vvp_context_t context)
{
      أكد(ptr.پورٹ() == 0);
      أكد(ناپ_2 == vwid);
      أكد(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_شے(context, context_idx_));

      بنانشانی wid = ثنائی_ہندسہ.size();
      لكل (بنانشانی idx = 0 ;  idx < wid ;  idx += 1) {
            إذا (ادھار+idx >= bits4->size()) اكسر;
            bits4->سیٹ_ثنائی_ہندسہ(ادھار+idx, ثنائی_ہندسہ.value(idx));
      }
      ptr.ptr()->send_vec4(*bits4, context);
}

بنانشانی vvp_fun_signal4_aa::قدر_ناپ() ثابت
{
      أرجع ناپ_2;
}

vvp_bit4_t vvp_fun_signal4_aa::value(بنانشانی idx) ثابت
{
      ثابت vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_شے(context_idx_));

      أرجع bits4->value(idx);
}

vvp_scalar_t vvp_fun_signal4_aa::scalar_قدر(بنانشانی idx) ثابت
{
      ثابت vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_شے(context_idx_));

      أرجع vvp_scalar_t(bits4->value(idx), 6, 6);
}

فراغ vvp_fun_signal4_aa::vec4_قدر(vvp_vector4_t&val) ثابت
{
      ثابت vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_شے(context_idx_));

      val = *bits4;
}

ثابت vvp_vector4_t&vvp_fun_signal4_aa::vec4_unfiltered_قدر() ثابت
{
      ثابت vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_شے(context_idx_));

      أرجع *bits4;
}

فراغ vvp_fun_signal4_aa::عامل مٹاؤ(فراغ*)
{
      أكد(0);
}

/*
 * Testing for equality, we want a bitwise test instead of an
 * arithmetic test because we want to treat for example -0 different
 * from +0.
 */
منطقي بٹ_equal(مزدوج a, مزدوج b)
{
      أرجع memcmp(&a, &b, حجم a) == 0;
}

vvp_fun_signal_real_sa::vvp_fun_signal_real_sa()
{
      بٹ_2 = 0.0;
}

مزدوج vvp_fun_signal_real_sa::real_unfiltered_قدر() ثابت
{
      أرجع بٹ_2;
}

فراغ vvp_fun_signal_real_sa::recv_real(vvp_net_ptr_t ptr, مزدوج ثنائی_ہندسہ,
                                       vvp_context_t)
{
      بدل (ptr.پورٹ()) {
	  حالة 0:
	    إذا (!continuous_سونپو_سرگرم) {
                  إذا (needs_init_ || !بٹ_equal(بٹ_2, ثنائی_ہندسہ)) {
			بٹ_2 = ثنائی_ہندسہ;
			needs_init_ = خطأ;
			ptr.ptr()->send_real(ثنائی_ہندسہ, 0);
		  }
	    }
	    اكسر;

	  حالة 1: // Continuous assign value
	    continuous_سونپو_سرگرم = سچ;
	    بٹ_2 = ثنائی_ہندسہ;
	    ptr.ptr()->send_real(ثنائی_ہندسہ, 0);
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.پورٹ());
	    أكد(0);
	    اكسر;
      }
}

vvp_fun_signal_real_aa::vvp_fun_signal_real_aa()
{
      context_idx_ = vpip_جوڑو_شے_منزل_context(هذا, vpip_peek_context_scope());
}

vvp_fun_signal_real_aa::~vvp_fun_signal_real_aa()
{
      أكد(0);
}

فراغ vvp_fun_signal_real_aa::alloc_instance(vvp_context_t context)
{
      مزدوج*بٹ = جديد مزدوج;
      vvp_سیٹ_context_شے(context, context_idx_, بٹ);

      *بٹ = 0.0;
}

فراغ vvp_fun_signal_real_aa::reset_instance(vvp_context_t context)
{
      مزدوج*بٹ = static_cast<مزدوج*>
            (vvp_get_context_شے(context, context_idx_));

      *بٹ = 0.0;
}

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
فراغ vvp_fun_signal_real_aa::free_instance(vvp_context_t context)
{
      مزدوج*بٹ = static_cast<مزدوج*>
            (vvp_get_context_شے(context, context_idx_));
      مٹاؤ بٹ;
}
#جے_انت

مزدوج vvp_fun_signal_real_aa::real_unfiltered_قدر() ثابت
{
      ثابت مزدوج*بٹ = static_cast<مزدوج*>
            (vthread_get_rd_context_شے(context_idx_));

      أرجع *بٹ;
}

مزدوج vvp_fun_signal_real_aa::real_قدر() ثابت
{
      أرجع real_unfiltered_قدر();
}

فراغ vvp_fun_signal_real_aa::recv_real(vvp_net_ptr_t ptr, مزدوج ثنائی_ہندسہ,
                                       vvp_context_t context)
{
      أكد(ptr.پورٹ() == 0);
      أكد(context);

      مزدوج*بٹ = static_cast<مزدوج*>
            (vvp_get_context_شے(context, context_idx_));

      إذا (!بٹ_equal(*بٹ,ثنائی_ہندسہ)) {
            *بٹ = ثنائی_ہندسہ;
            ptr.ptr()->send_real(ثنائی_ہندسہ, context);
      }
}

بنانشانی vvp_fun_signal_real_aa::قدر_ناپ() ثابت
{
      أكد(0);
      أرجع 1;
}

vvp_bit4_t vvp_fun_signal_real_aa::value(بنانشانی) ثابت
{
      أكد(0);
      أرجع BIT4_X;
}

vvp_scalar_t vvp_fun_signal_real_aa::scalar_قدر(بنانشانی) ثابت
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
      أرجع vvp_net_fun_t::ڈھیر.alloc(size);
}

فراغ vvp_fun_signal_real_aa::عامل مٹاؤ(فراغ*)
{
      أكد(0);
}


vvp_fun_signal_string_sa::vvp_fun_signal_string_sa()
{
}

فراغ vvp_fun_signal_string_sa::recv_string(vvp_net_ptr_t ptr, ثابت std::string&ثنائی_ہندسہ,
					   vvp_context_t)
{
      أكد(ptr.پورٹ() == 0);

      إذا (needs_init_ || قدر_2 != ثنائی_ہندسہ) {
	    قدر_2 = ثنائی_ہندسہ;
	    needs_init_ = خطأ;

	    ptr.ptr()->send_string(ثنائی_ہندسہ, 0);
      }
}

ثابت string& vvp_fun_signal_string_sa::get_string() ثابت
{
      أرجع قدر_2;
}

vvp_fun_signal_string_aa::vvp_fun_signal_string_aa()
{
      context_idx_ = vpip_جوڑو_شے_منزل_context(هذا, vpip_peek_context_scope());
}

vvp_fun_signal_string_aa::~vvp_fun_signal_string_aa()
{
      أكد(0);
}

فراغ vvp_fun_signal_string_aa::alloc_instance(vvp_context_t context)
{
      string*بٹ = جديد std::string;
      vvp_سیٹ_context_شے(context, context_idx_, بٹ);
      *بٹ = "";
}

فراغ vvp_fun_signal_string_aa::reset_instance(vvp_context_t context)
{
      string*بٹ = static_cast<std::string*>
	    (vvp_get_context_شے(context, context_idx_));
      *بٹ = "";
}

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
فراغ vvp_fun_signal_string_aa::free_instance(vvp_context_t context)
{
      string*بٹ = static_cast<std::string*>
            (vvp_get_context_شے(context, context_idx_));
      مٹاؤ بٹ;
}
#جے_انت

فراغ vvp_fun_signal_string_aa::recv_string(vvp_net_ptr_t ptr, ثابت std::string&ثنائی_ہندسہ, vvp_context_t context)
{
      أكد(ptr.پورٹ() == 0);
      أكد(context);

      string*بٹ = static_cast<std::string*>
	    (vvp_get_context_شے(context, context_idx_));

      إذا (*بٹ != ثنائی_ہندسہ) {
	    *بٹ = ثنائی_ہندسہ;
	    ptr.ptr()->send_string(ثنائی_ہندسہ, context);
      }
}

بنانشانی vvp_fun_signal_string_aa::قدر_ناپ() ثابت
{
      أكد(0);
      أرجع 1;
}

vvp_bit4_t vvp_fun_signal_string_aa::value(بنانشانی) ثابت
{
      أكد(0);
      أرجع BIT4_X;
}

vvp_scalar_t vvp_fun_signal_string_aa::scalar_قدر(بنانشانی) ثابت
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
      ثابت string*بٹ = static_cast<std::string*>
            (vthread_get_rd_context_شے(context_idx_));

      أرجع *بٹ;
}

فراغ* vvp_fun_signal_string_aa::عامل جديد(std::size_t size)
{
      أرجع vvp_net_fun_t::ڈھیر.alloc(size);
}

فراغ vvp_fun_signal_string_aa::عامل مٹاؤ(فراغ*)
{
      أكد(0);
}

  /* OBJECT signals */

vvp_fun_signal_object_sa::vvp_fun_signal_object_sa(بنانشانی size)
: vvp_fun_signal_object(size)
{
}

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
فراغ vvp_fun_signal_object_aa::free_instance(vvp_context_t context)
{
      vvp_object_t*بٹ = static_cast<vvp_object_t*>
            (vvp_get_context_شے(context, context_idx_));
      مٹاؤ بٹ;
}
#جے_انت

فراغ vvp_fun_signal_object_sa::recv_object(vvp_net_ptr_t ptr, vvp_object_t ثنائی_ہندسہ,
					   vvp_context_t)
{
      أكد(ptr.پورٹ() == 0);

      إذا (needs_init_ || قدر_2 != ثنائی_ہندسہ) {
	    قدر_2 = ثنائی_ہندسہ;
	    needs_init_ = خطأ;

	    ptr.ptr()->send_object(ثنائی_ہندسہ, 0);
      }
}

vvp_object_t vvp_fun_signal_object_sa::get_object() ثابت
{
      أرجع قدر_2;
}

vvp_fun_signal_object_aa::vvp_fun_signal_object_aa(بنانشانی size)
: vvp_fun_signal_object(size)
{
      context_idx_ = vpip_جوڑو_شے_منزل_context(هذا, vpip_peek_context_scope());
}

vvp_fun_signal_object_aa::~vvp_fun_signal_object_aa()
{
      أكد(0);
}

فراغ vvp_fun_signal_object_aa::alloc_instance(vvp_context_t context)
{
      vvp_object_t*بٹ = جديد vvp_object_t;
      vvp_سیٹ_context_شے(context, context_idx_, بٹ);
      بٹ->reset();
}

فراغ vvp_fun_signal_object_aa::reset_instance(vvp_context_t context)
{
      vvp_object_t*بٹ = static_cast<vvp_object_t*>
	    (vvp_get_context_شے(context, context_idx_));
      بٹ->reset();
}

vvp_object_t vvp_fun_signal_object_aa::get_object() ثابت
{
      ثابت vvp_object_t*بٹ = static_cast<vvp_object_t*>
	    (vthread_get_rd_context_شے(context_idx_));
      أرجع *بٹ;
}

فراغ vvp_fun_signal_object_aa::recv_object(vvp_net_ptr_t ptr, vvp_object_t ثنائی_ہندسہ,
					   vvp_context_t context)
{
      أكد(ptr.پورٹ() == 0);
      أكد(context);

      vvp_object_t*بٹ = static_cast<vvp_object_t*>
	    (vvp_get_context_شے(context, context_idx_));

      إذا (*بٹ != ثنائی_ہندسہ) {
	    *بٹ = ثنائی_ہندسہ;
	    ptr.ptr()->send_object(ثنائی_ہندسہ, context);
      }
}

بنانشانی vvp_fun_signal_object_aa::قدر_ناپ() ثابت
{
      أرجع 1;
}

vvp_bit4_t vvp_fun_signal_object_aa::value(بنانشانی) ثابت
{
      أكد(0);
      أرجع BIT4_X;
}

vvp_scalar_t vvp_fun_signal_object_aa::scalar_قدر(بنانشانی) ثابت
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
      أرجع vvp_net_fun_t::ڈھیر.alloc(size);
}

فراغ vvp_fun_signal_object_aa::عامل مٹاؤ(فراغ*)
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

فراغ vvp_fun_force::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			      vvp_context_t)
{
      أكد(ptr.پورٹ() == 0);
      vvp_net_t*net = ptr.ptr();

      vvp_net_t*dst = net->پورٹ[3].ptr();
      أكد(dst->fil);

      dst->force_vec4(coerce_منزل_چوڑائی(ثنائی_ہندسہ, dst->fil->filter_ناپ()), vvp_vector2_t(vvp_vector2_t::FILL1, dst->fil->filter_ناپ()));
}

فراغ vvp_fun_force::recv_real(vvp_net_ptr_t ptr, مزدوج ثنائی_ہندسہ, vvp_context_t)
{
      أكد(ptr.پورٹ() == 0);
      vvp_net_t*net = ptr.ptr();
      vvp_net_t*dst = net->پورٹ[3].ptr();
      dst->force_real(ثنائی_ہندسہ, vvp_vector2_t(vvp_vector2_t::FILL1, 1));
}

vvp_wire_ادھار::vvp_wire_ادھار()
{
}

vvp_wire_ادھار::~vvp_wire_ادھار()
{
}

vvp_bit4_t vvp_wire_ادھار::driven_قدر(بنانشانی) ثابت
{
      أكد(0);
      أرجع BIT4_X;
}

منطقي vvp_wire_ادھار::is_forced(بنانشانی) ثابت
{
      أكد(0);
      أرجع خطأ;
}

vvp_wire_vec4::vvp_wire_vec4(بنانشانی wid, vvp_bit4_t init)
: bits4_(wid, init)
{
      needs_init_ = سچ;
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec4(ثابت vvp_vector4_t&ثنائی_ہندسہ, vvp_vector4_t&rep,
						 بنانشانی ادھار, بنانشانی vwid)
{
	// Special case! the input bit is 0 wid. Interpret this as a
	// vector of BIT4_X to match the width of the bits4_ vector.
	// FIXME! This is a hack to work around some buggy gate
	// implementations! This should be removed!
      إذا (ادھار==0 && vwid==0) {
	    vvp_vector4_t tmp (bits4_.size(), BIT4_X);
	    إذا (bits4_ .eeq(tmp) && !needs_init_) أرجع STOP;
	    bits4_ = tmp;
	    needs_init_ = خطأ;
	    أرجع filter_نقاب(tmp, force4_, rep, 0);
      }

      إذا (vwid != bits4_.size()) {
	    cerr << "Internal error: Input vector expected width="
		 << bits4_.size() << ", got "
		 << "bit=" << ثنائی_ہندسہ << ", base=" << ادھار << ", vwid=" << vwid
		 << سطر_انت;
      }
      أكد(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      إذا (ادھار==0 && ثنائی_ہندسہ.size()==vwid) {
	    إذا (bits4_ .eeq( ثنائی_ہندسہ ) && !needs_init_) أرجع STOP;
	    bits4_ = ثنائی_ہندسہ;
      } وإلا {
	    منطقي rc = bits4_.سیٹ_vec(ادھار, ثنائی_ہندسہ);
	    إذا (rc == خطأ && !needs_init_) أرجع STOP;
      }

      needs_init_ = خطأ;
      أرجع filter_نقاب(ثنائی_ہندسہ, force4_, rep, ادھار);
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec8(ثابت vvp_vector8_t&ثنائی_ہندسہ,
                                                 vvp_vector8_t&rep,
                                                 بنانشانی ادھار,
                                                 بنانشانی vwid)
{
      أكد(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      vvp_vector4_t bit4 (reduce4(ثنائی_ہندسہ));
      إذا (ادھار==0 && bit4.size()==vwid) {
	    إذا (bits4_ .eeq( bit4 ) && !needs_init_) أرجع STOP;
	    bits4_ = bit4;
      } وإلا {
	    منطقي rc = bits4_.سیٹ_vec(ادھار, bit4);
	    إذا (rc == خطأ && !needs_init_) أرجع STOP;
      }

      needs_init_ = خطأ;
      أرجع filter_نقاب(ثنائی_ہندسہ, vvp_vector8_t(force4_,6,6), rep, ادھار);
}

بنانشانی vvp_wire_vec4::filter_ناپ() ثابت
{
      أرجع bits4_.size();
}

فراغ vvp_wire_vec4::force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&نقاب)
{
      force_نقاب(نقاب);

      إذا (force4_.size() == 0) {
	    force4_ = val;
      } وإلا {
	    لكل (بنانشانی idx = 0; idx < نقاب.size() ; idx += 1) {
		  إذا (نقاب.value(idx) == 0)
			استمر;

		  force4_.سیٹ_ثنائی_ہندسہ(idx, val.value(idx));
	    }
      }
      لگاتار_vpi_callbacks();
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
	    لگاتار_vpi_callbacks();
      } وإلا {
	      // Variables keep the current value.
	    vvp_vector4_t res (bits4_.size());
	    لكل (بنانشانی idx=0; idx<bits4_.size(); idx += 1)
		  res.سیٹ_ثنائی_ہندسہ(idx,value(idx));
            release_نقاب(نقاب);
	    ptr.ptr()->fun->recv_vec4(ptr, res, 0);
      }
}

فراغ vvp_wire_vec4::release_pv(vvp_net_ptr_t ptr, بنانشانی ادھار, بنانشانی wid, منطقي net_flag)
{
      أكد(bits4_.size() >= ادھار + wid);

      vvp_vector2_t نقاب (vvp_vector2_t::FILL0, bits4_.size());
      لكل (بنانشانی idx = 0 ; idx < wid ; idx += 1)
	    نقاب.سیٹ_ثنائی_ہندسہ(ادھار+idx, 1);

      إذا (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_نقاب(نقاب);
	    needs_init_ = ! force4_.subvalue(ادھار,wid) .eeq(bits4_.subvalue(ادھار,wid));
	    ptr.ptr()->send_vec4_pv(bits4_.subvalue(ادھار,wid),
				    ادھار, bits4_.size(), 0);
	    لگاتار_vpi_callbacks();
      } وإلا {
	      // Variables keep the current value.
	    vvp_vector4_t res (wid);
	    لكل (بنانشانی idx=0; idx<wid; idx += 1)
		  res.سیٹ_ثنائی_ہندسہ(idx,value(ادھار+idx));
	    release_نقاب(نقاب);
	    ptr.ptr()->fun->recv_vec4_pv(ptr, res, ادھار, bits4_.size(), 0);
      }
}

بنانشانی vvp_wire_vec4::قدر_ناپ() ثابت
{
      أرجع bits4_.size();
}

vvp_bit4_t vvp_wire_vec4::filtered_قدر(بنانشانی idx) ثابت
{
      إذا (test_force_نقاب(idx))
	    أرجع force4_.value(idx);
      وإلا
	    أرجع bits4_.value(idx);
}

vvp_bit4_t vvp_wire_vec4::value(بنانشانی idx) ثابت
{
      أرجع filtered_قدر(idx);
}

vvp_scalar_t vvp_wire_vec4::scalar_قدر(بنانشانی idx) ثابت
{
      أرجع vvp_scalar_t(value(idx),6,6);
}

فراغ vvp_wire_vec4::vec4_قدر(vvp_vector4_t&val) ثابت
{
      val = bits4_;
      إذا (test_force_نقاب_is_zero())
	    أرجع;

      لكل (بنانشانی idx = 0 ; idx < bits4_.size() ; idx += 1)
	    val.سیٹ_ثنائی_ہندسہ(idx, filtered_قدر(idx));
}

vvp_bit4_t vvp_wire_vec4::driven_قدر(بنانشانی idx) ثابت
{
      أرجع bits4_.value(idx);
}

منطقي vvp_wire_vec4::is_forced(بنانشانی idx) ثابت
{
      أرجع test_force_نقاب(idx);
}

vvp_wire_vec8::vvp_wire_vec8(بنانشانی wid)
: bits8_(wid)
{
      needs_init_ = سچ;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec4(ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                                 vvp_vector4_t&rep,
                                                 بنانشانی ادھار,
                                                 بنانشانی vwid)
{
	// For now there is no support for a non-zero base.
      أكد(0 == ادھار);
      أكد(bits8_.size() == vwid);
      أكد(bits8_.size() == ثنائی_ہندسہ.size());
	// QUESTION: Is it really correct to propagate a vec4 if this
	// is a vec8 node? In fact, it is really possible for a vec4
	// value to get through to a vec8 filter?
      vvp_vector8_t rep8;
      prop_t rc = filter_vec8(vvp_vector8_t(ثنائی_ہندسہ,6,6), rep8, 0, vwid);
      إذا (rc == REPL)
	    rep = reduce4(rep8);

      needs_init_ = خطأ;
      أرجع rc;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec8(ثابت vvp_vector8_t&ثنائی_ہندسہ, vvp_vector8_t&rep, بنانشانی ادھار, بنانشانی vwid)
{
      أكد(vwid == bits8_.size());
	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      إذا (ادھار==0 && ثنائی_ہندسہ.size()==vwid) {
	    إذا (bits8_.eeq(ثنائی_ہندسہ) && !needs_init_) أرجع STOP;
	    bits8_ = ثنائی_ہندسہ;
      } وإلا {
	    إذا (bits8_.subvalue(ادھار, ثنائی_ہندسہ.size()).eeq(ثنائی_ہندسہ) &&
		!needs_init_) {
		  أرجع STOP;
	    }
	    إذا (bits8_.size() == 0)
		  bits8_ = vvp_vector8_t(vwid);
	    أكد(bits8_.size() == vwid);
	    bits8_.سیٹ_vec(ادھار, ثنائی_ہندسہ);
      }
      needs_init_ = خطأ;
      أرجع filter_نقاب(ثنائی_ہندسہ, force8_, rep, ادھار);
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_داخلہ_vec8(ثابت vvp_vector8_t&ثنائی_ہندسہ, vvp_vector8_t&rep) ثابت
{
      أرجع filter_داخلہ_نقاب(ثنائی_ہندسہ, force8_, rep);
}

بنانشانی vvp_wire_vec8::filter_ناپ() ثابت
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
	    لكل (بنانشانی idx = 0; idx < نقاب.size() ; idx += 1) {
		  إذا (نقاب.value(idx) == 0)
			استمر;

		  force8_.سیٹ_ثنائی_ہندسہ(idx, val.value(idx));
	    }
      }
      لگاتار_vpi_callbacks();
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

فراغ vvp_wire_vec8::release_pv(vvp_net_ptr_t ptr, بنانشانی ادھار, بنانشانی wid, منطقي net_flag)
{
      أكد(bits8_.size() >= ادھار + wid);

      vvp_vector2_t نقاب (vvp_vector2_t::FILL0, bits8_.size());
      لكل (بنانشانی idx = 0 ; idx < wid ; idx += 1)
	    نقاب.سیٹ_ثنائی_ہندسہ(ادھار+idx, 1);

      release_نقاب(نقاب);

      إذا (net_flag) {
	    needs_init_ = !force8_.subvalue(ادھار,wid) .eeq((bits8_.subvalue(ادھار,wid)));
	    ptr.ptr()->send_vec8_pv(bits8_.subvalue(ادھار,wid),
				    ادھار, bits8_.size());
	    لگاتار_vpi_callbacks();
      } وإلا {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    أكد(0);
//	    ptr.ptr()->fun->recv_vec8_pv(ptr, force8_.subvalue(base,wid),
//					 base, force8_.size());
      }
}

بنانشانی vvp_wire_vec8::قدر_ناپ() ثابت
{
      أرجع bits8_.size();
}

vvp_scalar_t vvp_wire_vec8::filtered_قدر(بنانشانی idx) ثابت
{
      إذا (test_force_نقاب(idx))
	    أرجع force8_.value(idx);
      وإلا
	    أرجع bits8_.value(idx);
}

vvp_bit4_t vvp_wire_vec8::value(بنانشانی idx) ثابت
{
      أرجع filtered_قدر(idx).value();
}

vvp_scalar_t vvp_wire_vec8::scalar_قدر(بنانشانی idx) ثابت
{
      أرجع filtered_قدر(idx);
}

vvp_vector8_t vvp_wire_vec8::vec8_قدر() ثابت
{
      vvp_vector8_t tmp = bits8_;
      لكل (بنانشانی idx = 0 ; idx < bits8_.size() ; idx += 1)
	    tmp.سیٹ_ثنائی_ہندسہ(idx, filtered_قدر(idx));
      أرجع tmp;
}

فراغ vvp_wire_vec8::vec4_قدر(vvp_vector4_t&val) ثابت
{
      val = reduce4(vec8_قدر());
}

vvp_bit4_t vvp_wire_vec8::driven_قدر(بنانشانی idx) ثابت
{
      أرجع bits8_.value(idx).value();
}

منطقي vvp_wire_vec8::is_forced(بنانشانی idx) ثابت
{
      أرجع test_force_نقاب(idx);
}

vvp_wire_real::vvp_wire_real()
: ثنائی_ہندسہ_2(0.0), force_(0.0)
{
}

vvp_net_fil_t::prop_t vvp_wire_real::filter_real(مزدوج&ثنائی_ہندسہ)
{
      إذا (بٹ_equal(ثنائی_ہندسہ_2, ثنائی_ہندسہ) && !needs_init_) أرجع STOP;
      ثنائی_ہندسہ_2 = ثنائی_ہندسہ;
      needs_init_ = خطأ;
      أرجع filter_نقاب(ثنائی_ہندسہ, force_);
}

بنانشانی vvp_wire_real::filter_ناپ() ثابت
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

      لگاتار_vpi_callbacks();
}

فراغ vvp_wire_real::release(vvp_net_ptr_t ptr, منطقي net_flag)
{
      vvp_vector2_t نقاب (vvp_vector2_t::FILL1, 1);
      إذا (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_نقاب(نقاب);
	    needs_init_ = !بٹ_equal(force_, ثنائی_ہندسہ_2);
	    ptr.ptr()->send_real(ثنائی_ہندسہ_2, 0);
      } وإلا {
	      // Variables keep the current value.
	    مزدوج res =  real_قدر();
	    release_نقاب(نقاب);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
}

فراغ vvp_wire_real::release_pv(vvp_net_ptr_t, بنانشانی, بنانشانی, منطقي)
{
      أكد(0);
#إذا 0
	// A real is a single value. If for some reason this part release
	// can happen the following code should work correctly (requires
	// a base of 0 and a width of 1).
      vvp_vector2_t نقاب (vvp_vector2_t::FILL1, 1);
      أكد(ادھار == 0 && wid == 1);

      إذا (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_نقاب(نقاب);
	    ptr.ptr()->send_real(ثنائی_ہندسہ_2, 0);
      } وإلا {
	      // Variables keep the current value.
	    مزدوج res =  real_قدر();
	    release_نقاب(نقاب);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
#جے_انت
}

بنانشانی vvp_wire_real::قدر_ناپ() ثابت
{
      أكد(0);
      أرجع 1;
}

vvp_bit4_t vvp_wire_real::value(بنانشانی) ثابت
{
      أكد(0);
      أرجع BIT4_X;
}

vvp_scalar_t vvp_wire_real::scalar_قدر(بنانشانی) ثابت
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
	    أرجع ثنائی_ہندسہ_2;
}

#إذا 0
vvp_wire_string::vvp_wire_string()
{
}

بنانشانی vvp_wire_string::filter_ناپ() ثابت
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

فراغ vvp_wire_string::release_pv(vvp_net_ptr_t, بنانشانی, بنانشانی, منطقي)
{
      أكد(0);
}

بنانشانی vvp_wire_string::قدر_ناپ() ثابت
{
      أكد(0);
      أرجع 1;
}

vvp_bit4_t vvp_wire_string::value(بنانشانی) ثابت
{
      أكد(0);
      أرجع BIT4_X;
}

vvp_scalar_t vvp_wire_string::scalar_قدر(بنانشانی) ثابت
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
#جے_انت
