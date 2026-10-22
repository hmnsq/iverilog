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
#اگر_تعریف_شده CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
# include  <map>
#پایان_اگر

# include  <iostream>

استفاده نطاق std;

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
قالب <صنف T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_ماسک(ثابت T&val, ثابت T&force, T&filter, بیعلامت پایه)
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
	    أكد((پایه+val.size()) <= force_ماسک_2.size());

	    filter = val;
	    لكل (بیعلامت idx = 0 ; idx < val.size() ; idx += 1) {
		  إذا (force_ماسک_2.value(پایه+idx))
			filter.مجموعه_بیت(idx, force.value(پایه+idx));
		  وإلا
			propagate_flag = درست;
	    }

	    إذا (propagate_flag) {
		  پیاپی_vpi_callbacks();
		  أرجع REPL;
	    } وإلا {
		  أرجع STOP;
	    }

      } وإلا {
	    پیاپی_vpi_callbacks();
	    أرجع PROP;
      }
}

قالب <صنف T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_ماسک(T&val, T force)
{

      إذا (test_force_ماسک(0)) {
	    val = force;
	    پیاپی_vpi_callbacks();
	    أرجع REPL;
      }
      پیاپی_vpi_callbacks();
      أرجع PROP;
}

قالب <صنف T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_درون_داد_ماسک(ثابت T&val, ثابت T&force, T&rep) ثابت
{
      إذا (test_force_ماسک_is_zero())
	    أرجع PROP;

      أكد(force_ماسک_2.size() == force.size());

      rep = val;
      لكل (بیعلامت idx = 0 ; idx < val.size() ; idx += 1) {
	    إذا (force_ماسک_2.value(idx))
		  rep.مجموعه_بیت(idx, force.value(idx));
      }

      أرجع REPL;
}

vvp_signal_مقدار::~vvp_signal_مقدار()
{
}

مزدوج vvp_signal_مقدار::real_مقدار() ثابت
{
      أكد(0);
      أرجع 0;
}

فراغ vvp_net_t::force_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&ماسک)
{
      أكد(fil);
      fil->force_fil_vec4(val, ماسک);
      fun->force_flag(خطأ);
      vvp_send_vec4(بیرون_2, val, 0);
}

فراغ vvp_net_t::force_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&ماسک)
{
      أكد(fil);
      fil->force_fil_vec8(val, ماسک);
      fun->force_flag(خطأ);
      vvp_send_vec8(بیرون_2, val);
}

فراغ vvp_net_t::force_real(مزدوج val, ثابت vvp_vector2_t&ماسک)
{
      أكد(fil);
      fil->force_fil_real(val, ماسک);
      fun->force_flag(خطأ);
      vvp_send_real(بیرون_2, val, 0);
}

/* **** vvp_fun_signal methods **** */

vvp_fun_signal_پایه::vvp_fun_signal_پایه()
{
      continuous_تخصیص_بده_فعال = خطأ;
      needs_init_ = درست;
      cassign_link = 0;
      شمار_functors_sig += 1;
}

vvp_fun_signal4_sa::vvp_fun_signal4_sa(بیعلامت wid, vvp_bit4_t init)
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
فراغ vvp_fun_signal4_sa::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                                   vvp_context_t)
{
      بدل (ptr.پورت()) {
	  حالة 0: // Normal input (feed from net, or set from process)
	      /* If we don't have a continuous assign mask then just
		 copy the bits, otherwise we need to see if there are
		 any holes in the mask so we can set those bits. */
	    إذا (تخصیص_بده_ماسک.size() == 0) {
                  إذا (needs_init_ || !bits4_.eeq(بیت)) {
			أكد(بیت.size() == bits4_.size());
			bits4_ = بیت;
			needs_init_ = خطأ;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    } وإلا {
		  منطقي تغییریافته = خطأ;
		  أكد(bits4_.size() == تخصیص_بده_ماسک.size());
		  لكل (بیعلامت idx = 0 ;  idx < بیت.size() ;  idx += 1) {
			إذا (idx >= bits4_.size()) اكسر;
			إذا (تخصیص_بده_ماسک.value(idx)) استمر;
			bits4_.مجموعه_بیت(idx, بیت.value(idx));
			تغییریافته = درست;
		  }
		  إذا (تغییریافته) {
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
	    bits4_ = coerce_مقصد_عرض(بیت, bits4_.size());
	    تخصیص_بده_ماسک = vvp_vector2_t(vvp_vector2_t::FILL1, bits4_.size());
	    ptr.ptr()->send_vec4(bits4_, 0);
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.پورت());
	    أكد(0);
	    اكسر;
      }
}

فراغ vvp_fun_signal4_sa::recv_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&بیت)
{
      recv_vec4(ptr, reduce4(بیت), 0);
}

فراغ vvp_fun_signal4_sa::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
				      بیعلامت پایه, بیعلامت vwid, vvp_context_t)
{
      أكد(bits4_.size() == vwid);
      بیعلامت wid = بیت.size();

      بدل (ptr.پورت()) {
	  حالة 0: // Normal input
	    إذا (تخصیص_بده_ماسک.size() == 0) {
                  لكل (بیعلامت idx = 0 ;  idx < wid ;  idx += 1) {
			إذا (پایه+idx >= bits4_.size()) اكسر;
			bits4_.مجموعه_بیت(پایه+idx, بیت.value(idx));
		  }
		  needs_init_ = خطأ;
		  ptr.ptr()->send_vec4(bits4_,0);
	    } وإلا {
		  منطقي تغییریافته = خطأ;
		  أكد(bits4_.size() == تخصیص_بده_ماسک.size());
		  لكل (بیعلامت idx = 0 ;  idx < wid ;  idx += 1) {
			إذا (پایه+idx >= bits4_.size()) اكسر;
			إذا (تخصیص_بده_ماسک.value(پایه+idx)) استمر;
			bits4_.مجموعه_بیت(پایه+idx, بیت.value(idx));
			تغییریافته = درست;
		  }
		  إذا (تغییریافته) {
			needs_init_ = خطأ;
			ptr.ptr()->send_vec4(bits4_,0);
		  }
	    }
	    اكسر;

	  حالة 1: // Continuous assign value
	    إذا (تخصیص_بده_ماسک.size() == 0)
		  تخصیص_بده_ماسک = vvp_vector2_t(vvp_vector2_t::FILL0, bits4_.size());
	    لكل (بیعلامت idx = 0 ;  idx < wid ;  idx += 1) {
		  إذا (پایه+idx >= bits4_.size())
			اكسر;
		  bits4_.مجموعه_بیت(پایه+idx, بیت.value(idx));
		  تخصیص_بده_ماسک.مجموعه_بیت(پایه+idx, 1);
	    }
	    ptr.ptr()->send_vec4(bits4_,0);
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.پورت());
	    أكد(0);
	    اكسر;
      }
}

فراغ vvp_fun_signal4_sa::recv_vec8_pv(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&بیت,
				      بیعلامت پایه, بیعلامت vwid)
{
      recv_vec4_pv(ptr, reduce4(بیت), پایه, vwid, 0);
}

فراغ vvp_fun_signal_پایه::deassign()
{
      continuous_تخصیص_بده_فعال = خطأ;
      تخصیص_بده_ماسک = vvp_vector2_t();
}

فراغ vvp_fun_signal_پایه::deassign_pv(بیعلامت پایه, بیعلامت wid)
{
      لكل (بیعلامت idx = 0 ;  idx < wid ;  idx += 1) {
	    تخصیص_بده_ماسک.مجموعه_بیت(پایه+idx, 0);
      }

      إذا (تخصیص_بده_ماسک.is_zero()) {
	    تخصیص_بده_ماسک = vvp_vector2_t();
      }
}

فراغ automatic_signal_پایه::release(vvp_net_ptr_t,منطقي)
{
      أكد(0);
}

فراغ automatic_signal_پایه::release_pv(vvp_net_ptr_t,بیعلامت,بیعلامت,منطقي)
{
      أكد(0);
}

بیعلامت automatic_signal_پایه::filter_اندازه() ثابت
{
      أكد(0);
      أرجع(0);
}
فراغ automatic_signal_پایه::force_fil_vec4(ثابت vvp_vector4_t&, ثابت vvp_vector2_t&)
{
      أكد(0);
}
فراغ automatic_signal_پایه::force_fil_vec8(ثابت vvp_vector8_t&, ثابت vvp_vector2_t&)
{
      أكد(0);
}
فراغ automatic_signal_پایه::force_fil_real(مزدوج, ثابت vvp_vector2_t&)
{
      أكد(0);
}
فراغ automatic_signal_پایه::get_مقدار(بنية t_vpi_مقدار*)
{
      أكد(0);
}

ثابت vvp_vector4_t& vvp_fun_signal4_sa::vec4_unfiltered_مقدار() ثابت
{
      أرجع bits4_;
}

vvp_fun_signal4_aa::vvp_fun_signal4_aa(بیعلامت wid, vvp_bit4_t init)
{
      context_idx_ = vpip_جمع_کن_قلم_مقصد_context(هذا, vpip_peek_context_scope());
      اندازه_2 = wid;
      init_ = init;
}

vvp_fun_signal4_aa::~vvp_fun_signal4_aa()
{
      أكد(0);
}

فراغ vvp_fun_signal4_aa::alloc_instance(vvp_context_t context)
{
      vvp_مجموعه_context_قلم(context, context_idx_, جديد vvp_vector4_t(اندازه_2, init_));
}

فراغ vvp_fun_signal4_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*بیتها = static_cast<vvp_vector4_t*>
            (vvp_get_context_قلم(context, context_idx_));

      بیتها->fill_بیتها(init_);
}

#اگر_تعریف_شده CHECK_WITH_VALGRIND
فراغ vvp_fun_signal4_aa::free_instance(vvp_context_t context)
{
      vvp_vector4_t*بیتها = static_cast<vvp_vector4_t*>
            (vvp_get_context_قلم(context, context_idx_));
      حذف بیتها;
}
#پایان_اگر

/*
 * Continuous and forced assignments are not permitted on automatic
 * variables. So we only expect to receive on port 0.
 */
فراغ vvp_fun_signal4_aa::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                                   vvp_context_t context)
{
      أكد(ptr.پورت() == 0);
      أكد(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_قلم(context, context_idx_));

      إذا (!bits4->eeq(بیت)) {
            *bits4 = بیت;
            ptr.ptr()->send_vec4(*bits4, context);
      }
}

فراغ vvp_fun_signal4_aa::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
				      بیعلامت پایه, بیعلامت vwid, vvp_context_t context)
{
      أكد(ptr.پورت() == 0);
      أكد(اندازه_2 == vwid);
      أكد(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_قلم(context, context_idx_));

      بیعلامت wid = بیت.size();
      لكل (بیعلامت idx = 0 ;  idx < wid ;  idx += 1) {
            إذا (پایه+idx >= bits4->size()) اكسر;
            bits4->مجموعه_بیت(پایه+idx, بیت.value(idx));
      }
      ptr.ptr()->send_vec4(*bits4, context);
}

بیعلامت vvp_fun_signal4_aa::مقدار_اندازه() ثابت
{
      أرجع اندازه_2;
}

vvp_bit4_t vvp_fun_signal4_aa::value(بیعلامت idx) ثابت
{
      ثابت vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_قلم(context_idx_));

      أرجع bits4->value(idx);
}

vvp_scalar_t vvp_fun_signal4_aa::scalar_مقدار(بیعلامت idx) ثابت
{
      ثابت vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_قلم(context_idx_));

      أرجع vvp_scalar_t(bits4->value(idx), 6, 6);
}

فراغ vvp_fun_signal4_aa::vec4_مقدار(vvp_vector4_t&val) ثابت
{
      ثابت vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_قلم(context_idx_));

      val = *bits4;
}

ثابت vvp_vector4_t&vvp_fun_signal4_aa::vec4_unfiltered_مقدار() ثابت
{
      ثابت vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_قلم(context_idx_));

      أرجع *bits4;
}

فراغ vvp_fun_signal4_aa::عملگر حذف(فراغ*)
{
      أكد(0);
}

/*
 * Testing for equality, we want a bitwise test instead of an
 * arithmetic test because we want to treat for example -0 different
 * from +0.
 */
منطقي بیتها_equal(مزدوج a, مزدوج b)
{
      أرجع memcmp(&a, &b, حجم a) == 0;
}

vvp_fun_signal_real_sa::vvp_fun_signal_real_sa()
{
      بیتها_2 = 0.0;
}

مزدوج vvp_fun_signal_real_sa::real_unfiltered_مقدار() ثابت
{
      أرجع بیتها_2;
}

فراغ vvp_fun_signal_real_sa::recv_real(vvp_net_ptr_t ptr, مزدوج بیت,
                                       vvp_context_t)
{
      بدل (ptr.پورت()) {
	  حالة 0:
	    إذا (!continuous_تخصیص_بده_فعال) {
                  إذا (needs_init_ || !بیتها_equal(بیتها_2, بیت)) {
			بیتها_2 = بیت;
			needs_init_ = خطأ;
			ptr.ptr()->send_real(بیت, 0);
		  }
	    }
	    اكسر;

	  حالة 1: // Continuous assign value
	    continuous_تخصیص_بده_فعال = درست;
	    بیتها_2 = بیت;
	    ptr.ptr()->send_real(بیت, 0);
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.پورت());
	    أكد(0);
	    اكسر;
      }
}

vvp_fun_signal_real_aa::vvp_fun_signal_real_aa()
{
      context_idx_ = vpip_جمع_کن_قلم_مقصد_context(هذا, vpip_peek_context_scope());
}

vvp_fun_signal_real_aa::~vvp_fun_signal_real_aa()
{
      أكد(0);
}

فراغ vvp_fun_signal_real_aa::alloc_instance(vvp_context_t context)
{
      مزدوج*بیتها = جديد مزدوج;
      vvp_مجموعه_context_قلم(context, context_idx_, بیتها);

      *بیتها = 0.0;
}

فراغ vvp_fun_signal_real_aa::reset_instance(vvp_context_t context)
{
      مزدوج*بیتها = static_cast<مزدوج*>
            (vvp_get_context_قلم(context, context_idx_));

      *بیتها = 0.0;
}

#اگر_تعریف_شده CHECK_WITH_VALGRIND
فراغ vvp_fun_signal_real_aa::free_instance(vvp_context_t context)
{
      مزدوج*بیتها = static_cast<مزدوج*>
            (vvp_get_context_قلم(context, context_idx_));
      حذف بیتها;
}
#پایان_اگر

مزدوج vvp_fun_signal_real_aa::real_unfiltered_مقدار() ثابت
{
      ثابت مزدوج*بیتها = static_cast<مزدوج*>
            (vthread_get_rd_context_قلم(context_idx_));

      أرجع *بیتها;
}

مزدوج vvp_fun_signal_real_aa::real_مقدار() ثابت
{
      أرجع real_unfiltered_مقدار();
}

فراغ vvp_fun_signal_real_aa::recv_real(vvp_net_ptr_t ptr, مزدوج بیت,
                                       vvp_context_t context)
{
      أكد(ptr.پورت() == 0);
      أكد(context);

      مزدوج*بیتها = static_cast<مزدوج*>
            (vvp_get_context_قلم(context, context_idx_));

      إذا (!بیتها_equal(*بیتها,بیت)) {
            *بیتها = بیت;
            ptr.ptr()->send_real(بیت, context);
      }
}

بیعلامت vvp_fun_signal_real_aa::مقدار_اندازه() ثابت
{
      أكد(0);
      أرجع 1;
}

vvp_bit4_t vvp_fun_signal_real_aa::value(بیعلامت) ثابت
{
      أكد(0);
      أرجع BIT4_X;
}

vvp_scalar_t vvp_fun_signal_real_aa::scalar_مقدار(بیعلامت) ثابت
{
      أكد(0);
      أرجع vvp_scalar_t();
}

فراغ vvp_fun_signal_real_aa::vec4_مقدار(vvp_vector4_t&) ثابت
{
      أكد(0);
}

فراغ* vvp_fun_signal_real_aa::عملگر جديد(std::size_t size)
{
      أرجع vvp_net_fun_t::انباشته.alloc(size);
}

فراغ vvp_fun_signal_real_aa::عملگر حذف(فراغ*)
{
      أكد(0);
}


vvp_fun_signal_string_sa::vvp_fun_signal_string_sa()
{
}

فراغ vvp_fun_signal_string_sa::recv_string(vvp_net_ptr_t ptr, ثابت std::string&بیت,
					   vvp_context_t)
{
      أكد(ptr.پورت() == 0);

      إذا (needs_init_ || مقدار_2 != بیت) {
	    مقدار_2 = بیت;
	    needs_init_ = خطأ;

	    ptr.ptr()->send_string(بیت, 0);
      }
}

ثابت string& vvp_fun_signal_string_sa::get_string() ثابت
{
      أرجع مقدار_2;
}

vvp_fun_signal_string_aa::vvp_fun_signal_string_aa()
{
      context_idx_ = vpip_جمع_کن_قلم_مقصد_context(هذا, vpip_peek_context_scope());
}

vvp_fun_signal_string_aa::~vvp_fun_signal_string_aa()
{
      أكد(0);
}

فراغ vvp_fun_signal_string_aa::alloc_instance(vvp_context_t context)
{
      string*بیتها = جديد std::string;
      vvp_مجموعه_context_قلم(context, context_idx_, بیتها);
      *بیتها = "";
}

فراغ vvp_fun_signal_string_aa::reset_instance(vvp_context_t context)
{
      string*بیتها = static_cast<std::string*>
	    (vvp_get_context_قلم(context, context_idx_));
      *بیتها = "";
}

#اگر_تعریف_شده CHECK_WITH_VALGRIND
فراغ vvp_fun_signal_string_aa::free_instance(vvp_context_t context)
{
      string*بیتها = static_cast<std::string*>
            (vvp_get_context_قلم(context, context_idx_));
      حذف بیتها;
}
#پایان_اگر

فراغ vvp_fun_signal_string_aa::recv_string(vvp_net_ptr_t ptr, ثابت std::string&بیت, vvp_context_t context)
{
      أكد(ptr.پورت() == 0);
      أكد(context);

      string*بیتها = static_cast<std::string*>
	    (vvp_get_context_قلم(context, context_idx_));

      إذا (*بیتها != بیت) {
	    *بیتها = بیت;
	    ptr.ptr()->send_string(بیت, context);
      }
}

بیعلامت vvp_fun_signal_string_aa::مقدار_اندازه() ثابت
{
      أكد(0);
      أرجع 1;
}

vvp_bit4_t vvp_fun_signal_string_aa::value(بیعلامت) ثابت
{
      أكد(0);
      أرجع BIT4_X;
}

vvp_scalar_t vvp_fun_signal_string_aa::scalar_مقدار(بیعلامت) ثابت
{
      أكد(0);
      أرجع vvp_scalar_t();
}

فراغ vvp_fun_signal_string_aa::vec4_مقدار(vvp_vector4_t&) ثابت
{
      أكد(0);
}

مزدوج vvp_fun_signal_string_aa::real_مقدار() ثابت
{
      أكد(0);
      أرجع 0.0;
}

ثابت std::string& vvp_fun_signal_string_aa::get_string() ثابت
{
      ثابت string*بیتها = static_cast<std::string*>
            (vthread_get_rd_context_قلم(context_idx_));

      أرجع *بیتها;
}

فراغ* vvp_fun_signal_string_aa::عملگر جديد(std::size_t size)
{
      أرجع vvp_net_fun_t::انباشته.alloc(size);
}

فراغ vvp_fun_signal_string_aa::عملگر حذف(فراغ*)
{
      أكد(0);
}

  /* OBJECT signals */

vvp_fun_signal_object_sa::vvp_fun_signal_object_sa(بیعلامت size)
: vvp_fun_signal_object(size)
{
}

#اگر_تعریف_شده CHECK_WITH_VALGRIND
فراغ vvp_fun_signal_object_aa::free_instance(vvp_context_t context)
{
      vvp_object_t*بیتها = static_cast<vvp_object_t*>
            (vvp_get_context_قلم(context, context_idx_));
      حذف بیتها;
}
#پایان_اگر

فراغ vvp_fun_signal_object_sa::recv_object(vvp_net_ptr_t ptr, vvp_object_t بیت,
					   vvp_context_t)
{
      أكد(ptr.پورت() == 0);

      إذا (needs_init_ || مقدار_2 != بیت) {
	    مقدار_2 = بیت;
	    needs_init_ = خطأ;

	    ptr.ptr()->send_object(بیت, 0);
      }
}

vvp_object_t vvp_fun_signal_object_sa::get_object() ثابت
{
      أرجع مقدار_2;
}

vvp_fun_signal_object_aa::vvp_fun_signal_object_aa(بیعلامت size)
: vvp_fun_signal_object(size)
{
      context_idx_ = vpip_جمع_کن_قلم_مقصد_context(هذا, vpip_peek_context_scope());
}

vvp_fun_signal_object_aa::~vvp_fun_signal_object_aa()
{
      أكد(0);
}

فراغ vvp_fun_signal_object_aa::alloc_instance(vvp_context_t context)
{
      vvp_object_t*بیتها = جديد vvp_object_t;
      vvp_مجموعه_context_قلم(context, context_idx_, بیتها);
      بیتها->reset();
}

فراغ vvp_fun_signal_object_aa::reset_instance(vvp_context_t context)
{
      vvp_object_t*بیتها = static_cast<vvp_object_t*>
	    (vvp_get_context_قلم(context, context_idx_));
      بیتها->reset();
}

vvp_object_t vvp_fun_signal_object_aa::get_object() ثابت
{
      ثابت vvp_object_t*بیتها = static_cast<vvp_object_t*>
	    (vthread_get_rd_context_قلم(context_idx_));
      أرجع *بیتها;
}

فراغ vvp_fun_signal_object_aa::recv_object(vvp_net_ptr_t ptr, vvp_object_t بیت,
					   vvp_context_t context)
{
      أكد(ptr.پورت() == 0);
      أكد(context);

      vvp_object_t*بیتها = static_cast<vvp_object_t*>
	    (vvp_get_context_قلم(context, context_idx_));

      إذا (*بیتها != بیت) {
	    *بیتها = بیت;
	    ptr.ptr()->send_object(بیت, context);
      }
}

بیعلامت vvp_fun_signal_object_aa::مقدار_اندازه() ثابت
{
      أرجع 1;
}

vvp_bit4_t vvp_fun_signal_object_aa::value(بیعلامت) ثابت
{
      أكد(0);
      أرجع BIT4_X;
}

vvp_scalar_t vvp_fun_signal_object_aa::scalar_مقدار(بیعلامت) ثابت
{
      أكد(0);
      أرجع vvp_scalar_t();
}

فراغ vvp_fun_signal_object_aa::vec4_مقدار(vvp_vector4_t&) ثابت
{
      أكد(0);
}

فراغ* vvp_fun_signal_object_aa::عملگر جديد(std::size_t size)
{
      أرجع vvp_net_fun_t::انباشته.alloc(size);
}

فراغ vvp_fun_signal_object_aa::عملگر حذف(فراغ*)
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

فراغ vvp_fun_force::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
			      vvp_context_t)
{
      أكد(ptr.پورت() == 0);
      vvp_net_t*net = ptr.ptr();

      vvp_net_t*dst = net->پورت[3].ptr();
      أكد(dst->fil);

      dst->force_vec4(coerce_مقصد_عرض(بیت, dst->fil->filter_اندازه()), vvp_vector2_t(vvp_vector2_t::FILL1, dst->fil->filter_اندازه()));
}

فراغ vvp_fun_force::recv_real(vvp_net_ptr_t ptr, مزدوج بیت, vvp_context_t)
{
      أكد(ptr.پورت() == 0);
      vvp_net_t*net = ptr.ptr();
      vvp_net_t*dst = net->پورت[3].ptr();
      dst->force_real(بیت, vvp_vector2_t(vvp_vector2_t::FILL1, 1));
}

vvp_wire_پایه::vvp_wire_پایه()
{
}

vvp_wire_پایه::~vvp_wire_پایه()
{
}

vvp_bit4_t vvp_wire_پایه::driven_مقدار(بیعلامت) ثابت
{
      أكد(0);
      أرجع BIT4_X;
}

منطقي vvp_wire_پایه::is_forced(بیعلامت) ثابت
{
      أكد(0);
      أرجع خطأ;
}

vvp_wire_vec4::vvp_wire_vec4(بیعلامت wid, vvp_bit4_t init)
: bits4_(wid, init)
{
      needs_init_ = درست;
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec4(ثابت vvp_vector4_t&بیت, vvp_vector4_t&rep,
						 بیعلامت پایه, بیعلامت vwid)
{
	// Special case! the input bit is 0 wid. Interpret this as a
	// vector of BIT4_X to match the width of the bits4_ vector.
	// FIXME! This is a hack to work around some buggy gate
	// implementations! This should be removed!
      إذا (پایه==0 && vwid==0) {
	    vvp_vector4_t tmp (bits4_.size(), BIT4_X);
	    إذا (bits4_ .eeq(tmp) && !needs_init_) أرجع STOP;
	    bits4_ = tmp;
	    needs_init_ = خطأ;
	    أرجع filter_ماسک(tmp, force4_, rep, 0);
      }

      إذا (vwid != bits4_.size()) {
	    cerr << "Internal error: Input vector expected width="
		 << bits4_.size() << ", got "
		 << "bit=" << بیت << ", base=" << پایه << ", vwid=" << vwid
		 << پایان_سطر;
      }
      أكد(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      إذا (پایه==0 && بیت.size()==vwid) {
	    إذا (bits4_ .eeq( بیت ) && !needs_init_) أرجع STOP;
	    bits4_ = بیت;
      } وإلا {
	    منطقي rc = bits4_.مجموعه_vec(پایه, بیت);
	    إذا (rc == خطأ && !needs_init_) أرجع STOP;
      }

      needs_init_ = خطأ;
      أرجع filter_ماسک(بیت, force4_, rep, پایه);
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec8(ثابت vvp_vector8_t&بیت,
                                                 vvp_vector8_t&rep,
                                                 بیعلامت پایه,
                                                 بیعلامت vwid)
{
      أكد(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      vvp_vector4_t bit4 (reduce4(بیت));
      إذا (پایه==0 && bit4.size()==vwid) {
	    إذا (bits4_ .eeq( bit4 ) && !needs_init_) أرجع STOP;
	    bits4_ = bit4;
      } وإلا {
	    منطقي rc = bits4_.مجموعه_vec(پایه, bit4);
	    إذا (rc == خطأ && !needs_init_) أرجع STOP;
      }

      needs_init_ = خطأ;
      أرجع filter_ماسک(بیت, vvp_vector8_t(force4_,6,6), rep, پایه);
}

بیعلامت vvp_wire_vec4::filter_اندازه() ثابت
{
      أرجع bits4_.size();
}

فراغ vvp_wire_vec4::force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&ماسک)
{
      force_ماسک(ماسک);

      إذا (force4_.size() == 0) {
	    force4_ = val;
      } وإلا {
	    لكل (بیعلامت idx = 0; idx < ماسک.size() ; idx += 1) {
		  إذا (ماسک.value(idx) == 0)
			استمر;

		  force4_.مجموعه_بیت(idx, val.value(idx));
	    }
      }
      پیاپی_vpi_callbacks();
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
	    پیاپی_vpi_callbacks();
      } وإلا {
	      // Variables keep the current value.
	    vvp_vector4_t res (bits4_.size());
	    لكل (بیعلامت idx=0; idx<bits4_.size(); idx += 1)
		  res.مجموعه_بیت(idx,value(idx));
            release_ماسک(ماسک);
	    ptr.ptr()->fun->recv_vec4(ptr, res, 0);
      }
}

فراغ vvp_wire_vec4::release_pv(vvp_net_ptr_t ptr, بیعلامت پایه, بیعلامت wid, منطقي net_flag)
{
      أكد(bits4_.size() >= پایه + wid);

      vvp_vector2_t ماسک (vvp_vector2_t::FILL0, bits4_.size());
      لكل (بیعلامت idx = 0 ; idx < wid ; idx += 1)
	    ماسک.مجموعه_بیت(پایه+idx, 1);

      إذا (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_ماسک(ماسک);
	    needs_init_ = ! force4_.subvalue(پایه,wid) .eeq(bits4_.subvalue(پایه,wid));
	    ptr.ptr()->send_vec4_pv(bits4_.subvalue(پایه,wid),
				    پایه, bits4_.size(), 0);
	    پیاپی_vpi_callbacks();
      } وإلا {
	      // Variables keep the current value.
	    vvp_vector4_t res (wid);
	    لكل (بیعلامت idx=0; idx<wid; idx += 1)
		  res.مجموعه_بیت(idx,value(پایه+idx));
	    release_ماسک(ماسک);
	    ptr.ptr()->fun->recv_vec4_pv(ptr, res, پایه, bits4_.size(), 0);
      }
}

بیعلامت vvp_wire_vec4::مقدار_اندازه() ثابت
{
      أرجع bits4_.size();
}

vvp_bit4_t vvp_wire_vec4::filtered_مقدار(بیعلامت idx) ثابت
{
      إذا (test_force_ماسک(idx))
	    أرجع force4_.value(idx);
      وإلا
	    أرجع bits4_.value(idx);
}

vvp_bit4_t vvp_wire_vec4::value(بیعلامت idx) ثابت
{
      أرجع filtered_مقدار(idx);
}

vvp_scalar_t vvp_wire_vec4::scalar_مقدار(بیعلامت idx) ثابت
{
      أرجع vvp_scalar_t(value(idx),6,6);
}

فراغ vvp_wire_vec4::vec4_مقدار(vvp_vector4_t&val) ثابت
{
      val = bits4_;
      إذا (test_force_ماسک_is_zero())
	    أرجع;

      لكل (بیعلامت idx = 0 ; idx < bits4_.size() ; idx += 1)
	    val.مجموعه_بیت(idx, filtered_مقدار(idx));
}

vvp_bit4_t vvp_wire_vec4::driven_مقدار(بیعلامت idx) ثابت
{
      أرجع bits4_.value(idx);
}

منطقي vvp_wire_vec4::is_forced(بیعلامت idx) ثابت
{
      أرجع test_force_ماسک(idx);
}

vvp_wire_vec8::vvp_wire_vec8(بیعلامت wid)
: bits8_(wid)
{
      needs_init_ = درست;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec4(ثابت vvp_vector4_t&بیت,
                                                 vvp_vector4_t&rep,
                                                 بیعلامت پایه,
                                                 بیعلامت vwid)
{
	// For now there is no support for a non-zero base.
      أكد(0 == پایه);
      أكد(bits8_.size() == vwid);
      أكد(bits8_.size() == بیت.size());
	// QUESTION: Is it really correct to propagate a vec4 if this
	// is a vec8 node? In fact, it is really possible for a vec4
	// value to get through to a vec8 filter?
      vvp_vector8_t rep8;
      prop_t rc = filter_vec8(vvp_vector8_t(بیت,6,6), rep8, 0, vwid);
      إذا (rc == REPL)
	    rep = reduce4(rep8);

      needs_init_ = خطأ;
      أرجع rc;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec8(ثابت vvp_vector8_t&بیت, vvp_vector8_t&rep, بیعلامت پایه, بیعلامت vwid)
{
      أكد(vwid == bits8_.size());
	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      إذا (پایه==0 && بیت.size()==vwid) {
	    إذا (bits8_.eeq(بیت) && !needs_init_) أرجع STOP;
	    bits8_ = بیت;
      } وإلا {
	    إذا (bits8_.subvalue(پایه, بیت.size()).eeq(بیت) &&
		!needs_init_) {
		  أرجع STOP;
	    }
	    إذا (bits8_.size() == 0)
		  bits8_ = vvp_vector8_t(vwid);
	    أكد(bits8_.size() == vwid);
	    bits8_.مجموعه_vec(پایه, بیت);
      }
      needs_init_ = خطأ;
      أرجع filter_ماسک(بیت, force8_, rep, پایه);
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_درون_داد_vec8(ثابت vvp_vector8_t&بیت, vvp_vector8_t&rep) ثابت
{
      أرجع filter_درون_داد_ماسک(بیت, force8_, rep);
}

بیعلامت vvp_wire_vec8::filter_اندازه() ثابت
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
	    لكل (بیعلامت idx = 0; idx < ماسک.size() ; idx += 1) {
		  إذا (ماسک.value(idx) == 0)
			استمر;

		  force8_.مجموعه_بیت(idx, val.value(idx));
	    }
      }
      پیاپی_vpi_callbacks();
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

فراغ vvp_wire_vec8::release_pv(vvp_net_ptr_t ptr, بیعلامت پایه, بیعلامت wid, منطقي net_flag)
{
      أكد(bits8_.size() >= پایه + wid);

      vvp_vector2_t ماسک (vvp_vector2_t::FILL0, bits8_.size());
      لكل (بیعلامت idx = 0 ; idx < wid ; idx += 1)
	    ماسک.مجموعه_بیت(پایه+idx, 1);

      release_ماسک(ماسک);

      إذا (net_flag) {
	    needs_init_ = !force8_.subvalue(پایه,wid) .eeq((bits8_.subvalue(پایه,wid)));
	    ptr.ptr()->send_vec8_pv(bits8_.subvalue(پایه,wid),
				    پایه, bits8_.size());
	    پیاپی_vpi_callbacks();
      } وإلا {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    أكد(0);
//	    ptr.ptr()->fun->recv_vec8_pv(ptr, force8_.subvalue(base,wid),
//					 base, force8_.size());
      }
}

بیعلامت vvp_wire_vec8::مقدار_اندازه() ثابت
{
      أرجع bits8_.size();
}

vvp_scalar_t vvp_wire_vec8::filtered_مقدار(بیعلامت idx) ثابت
{
      إذا (test_force_ماسک(idx))
	    أرجع force8_.value(idx);
      وإلا
	    أرجع bits8_.value(idx);
}

vvp_bit4_t vvp_wire_vec8::value(بیعلامت idx) ثابت
{
      أرجع filtered_مقدار(idx).value();
}

vvp_scalar_t vvp_wire_vec8::scalar_مقدار(بیعلامت idx) ثابت
{
      أرجع filtered_مقدار(idx);
}

vvp_vector8_t vvp_wire_vec8::vec8_مقدار() ثابت
{
      vvp_vector8_t tmp = bits8_;
      لكل (بیعلامت idx = 0 ; idx < bits8_.size() ; idx += 1)
	    tmp.مجموعه_بیت(idx, filtered_مقدار(idx));
      أرجع tmp;
}

فراغ vvp_wire_vec8::vec4_مقدار(vvp_vector4_t&val) ثابت
{
      val = reduce4(vec8_مقدار());
}

vvp_bit4_t vvp_wire_vec8::driven_مقدار(بیعلامت idx) ثابت
{
      أرجع bits8_.value(idx).value();
}

منطقي vvp_wire_vec8::is_forced(بیعلامت idx) ثابت
{
      أرجع test_force_ماسک(idx);
}

vvp_wire_real::vvp_wire_real()
: بیت_2(0.0), force_(0.0)
{
}

vvp_net_fil_t::prop_t vvp_wire_real::filter_real(مزدوج&بیت)
{
      إذا (بیتها_equal(بیت_2, بیت) && !needs_init_) أرجع STOP;
      بیت_2 = بیت;
      needs_init_ = خطأ;
      أرجع filter_ماسک(بیت, force_);
}

بیعلامت vvp_wire_real::filter_اندازه() ثابت
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

      پیاپی_vpi_callbacks();
}

فراغ vvp_wire_real::release(vvp_net_ptr_t ptr, منطقي net_flag)
{
      vvp_vector2_t ماسک (vvp_vector2_t::FILL1, 1);
      إذا (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_ماسک(ماسک);
	    needs_init_ = !بیتها_equal(force_, بیت_2);
	    ptr.ptr()->send_real(بیت_2, 0);
      } وإلا {
	      // Variables keep the current value.
	    مزدوج res =  real_مقدار();
	    release_ماسک(ماسک);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
}

فراغ vvp_wire_real::release_pv(vvp_net_ptr_t, بیعلامت, بیعلامت, منطقي)
{
      أكد(0);
#إذا 0
	// A real is a single value. If for some reason this part release
	// can happen the following code should work correctly (requires
	// a base of 0 and a width of 1).
      vvp_vector2_t ماسک (vvp_vector2_t::FILL1, 1);
      أكد(پایه == 0 && wid == 1);

      إذا (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_ماسک(ماسک);
	    ptr.ptr()->send_real(بیت_2, 0);
      } وإلا {
	      // Variables keep the current value.
	    مزدوج res =  real_مقدار();
	    release_ماسک(ماسک);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
#پایان_اگر
}

بیعلامت vvp_wire_real::مقدار_اندازه() ثابت
{
      أكد(0);
      أرجع 1;
}

vvp_bit4_t vvp_wire_real::value(بیعلامت) ثابت
{
      أكد(0);
      أرجع BIT4_X;
}

vvp_scalar_t vvp_wire_real::scalar_مقدار(بیعلامت) ثابت
{
      أكد(0);
      أرجع vvp_scalar_t();
}

فراغ vvp_wire_real::vec4_مقدار(vvp_vector4_t&) ثابت
{
      أكد(0);
}

مزدوج vvp_wire_real::real_مقدار() ثابت
{
      إذا (test_force_ماسک(0))
	    أرجع force_;
      وإلا
	    أرجع بیت_2;
}

#إذا 0
vvp_wire_string::vvp_wire_string()
{
}

بیعلامت vvp_wire_string::filter_اندازه() ثابت
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

فراغ vvp_wire_string::release_pv(vvp_net_ptr_t, بیعلامت, بیعلامت, منطقي)
{
      أكد(0);
}

بیعلامت vvp_wire_string::مقدار_اندازه() ثابت
{
      أكد(0);
      أرجع 1;
}

vvp_bit4_t vvp_wire_string::value(بیعلامت) ثابت
{
      أكد(0);
      أرجع BIT4_X;
}

vvp_scalar_t vvp_wire_string::scalar_مقدار(بیعلامت) ثابت
{
      أكد(0);
      أرجع vvp_scalar_t();
}

فراغ vvp_wire_string::vec4_مقدار(vvp_vector4_t&) ثابت
{
      أكد(0);
}

مزدوج vvp_wire_string::real_مقدار() ثابت
{
      أكد(0);
      أرجع 0.0;
}
#پایان_اگر
