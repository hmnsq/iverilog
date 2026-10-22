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
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
# include  <map>
#اگر_ختم

# include  <iostream>

استعمال نامخلا std;

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
سانچہ <جماعت T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_نقاب(ثابت T&val, ثابت T&force, T&filter, غیرعلامتی بنیاد)
{
      اگر (!test_force_نقاب_is_zero()) {
	      // Some bits are being forced. Go through the
	      // force_mask_ and force value to see which bits are
	      // propagated and which are kept from the forced
	      // value. Update the filter with the filtered result and
	      // return REPL to indicate that some bits have changed,
	      // or STOP if no bits change.
	    منطقی propagate_flag = force_propagate_;
	    force_propagate_ = جھوٹ;
	    أكد(force_نقاب_2.size() == force.size());
	    أكد((بنیاد+val.size()) <= force_نقاب_2.size());

	    filter = val;
	    برائے (غیرعلامتی idx = 0 ; idx < val.size() ; idx += 1) {
		  اگر (force_نقاب_2.value(بنیاد+idx))
			filter.سیٹ_ثنائی_ہندسہ(idx, force.value(بنیاد+idx));
		  ورنہ
			propagate_flag = سچ;
	    }

	    اگر (propagate_flag) {
		  تسلسل_vpi_callbacks();
		  واپس REPL;
	    } ورنہ {
		  واپس STOP;
	    }

      } ورنہ {
	    تسلسل_vpi_callbacks();
	    واپس PROP;
      }
}

سانچہ <جماعت T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_نقاب(T&val, T force)
{

      اگر (test_force_نقاب(0)) {
	    val = force;
	    تسلسل_vpi_callbacks();
	    واپس REPL;
      }
      تسلسل_vpi_callbacks();
      واپس PROP;
}

سانچہ <جماعت T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_داخلہ_نقاب(ثابت T&val, ثابت T&force, T&rep) ثابت
{
      اگر (test_force_نقاب_is_zero())
	    واپس PROP;

      أكد(force_نقاب_2.size() == force.size());

      rep = val;
      برائے (غیرعلامتی idx = 0 ; idx < val.size() ; idx += 1) {
	    اگر (force_نقاب_2.value(idx))
		  rep.سیٹ_ثنائی_ہندسہ(idx, force.value(idx));
      }

      واپس REPL;
}

vvp_signal_قدر::~vvp_signal_قدر()
{
}

دوہرا vvp_signal_قدر::real_قدر() ثابت
{
      أكد(0);
      واپس 0;
}

خالی vvp_net_t::force_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&نقاب)
{
      أكد(fil);
      fil->force_fil_vec4(val, نقاب);
      fun->force_flag(جھوٹ);
      vvp_send_vec4(برآمد_2, val, 0);
}

خالی vvp_net_t::force_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&نقاب)
{
      أكد(fil);
      fil->force_fil_vec8(val, نقاب);
      fun->force_flag(جھوٹ);
      vvp_send_vec8(برآمد_2, val);
}

خالی vvp_net_t::force_real(دوہرا val, ثابت vvp_vector2_t&نقاب)
{
      أكد(fil);
      fil->force_fil_real(val, نقاب);
      fun->force_flag(جھوٹ);
      vvp_send_real(برآمد_2, val, 0);
}

/* **** vvp_fun_signal methods **** */

vvp_fun_signal_بنیاد::vvp_fun_signal_بنیاد()
{
      continuous_تفویض_فعال = جھوٹ;
      needs_init_ = سچ;
      cassign_link = 0;
      گنتی_functors_sig += 1;
}

vvp_fun_signal4_sa::vvp_fun_signal4_sa(غیرعلامتی wid, vvp_bit4_t init)
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
خالی vvp_fun_signal4_sa::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                   vvp_context_t)
{
      انتخاب (ptr.پورٹ()) {
	  حالت 0: // Normal input (feed from net, or set from process)
	      /* If we don't have a continuous assign mask then just
		 copy the bits, otherwise we need to see if there are
		 any holes in the mask so we can set those bits. */
	    اگر (تفویض_نقاب.size() == 0) {
                  اگر (needs_init_ || !bits4_.eeq(ثنائی_ہندسہ)) {
			أكد(ثنائی_ہندسہ.size() == bits4_.size());
			bits4_ = ثنائی_ہندسہ;
			needs_init_ = جھوٹ;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    } ورنہ {
		  منطقی بدلا = جھوٹ;
		  أكد(bits4_.size() == تفویض_نقاب.size());
		  برائے (غیرعلامتی idx = 0 ;  idx < ثنائی_ہندسہ.size() ;  idx += 1) {
			اگر (idx >= bits4_.size()) توڑو;
			اگر (تفویض_نقاب.value(idx)) جاری;
			bits4_.سیٹ_ثنائی_ہندسہ(idx, ثنائی_ہندسہ.value(idx));
			بدلا = سچ;
		  }
		  اگر (بدلا) {
			needs_init_ = جھوٹ;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    }
	    توڑو;

	  حالت 1: // Continuous assign value
	      // Handle the simple case of the linked source being wider
	      // than this signal. Note we don't yet support the case of
	      // the linked source being narrower than this signal, or
	      // the case of an expression being assigned.
	    bits4_ = coerce_مقصد_چوڑائی(ثنائی_ہندسہ, bits4_.size());
	    تفویض_نقاب = vvp_vector2_t(vvp_vector2_t::FILL1, bits4_.size());
	    ptr.ptr()->send_vec4(bits4_, 0);
	    توڑو;

	  طےشدہ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.پورٹ());
	    أكد(0);
	    توڑو;
      }
}

خالی vvp_fun_signal4_sa::recv_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&ثنائی_ہندسہ)
{
      recv_vec4(ptr, reduce4(ثنائی_ہندسہ), 0);
}

خالی vvp_fun_signal4_sa::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				      غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t)
{
      أكد(bits4_.size() == vwid);
      غیرعلامتی wid = ثنائی_ہندسہ.size();

      انتخاب (ptr.پورٹ()) {
	  حالت 0: // Normal input
	    اگر (تفویض_نقاب.size() == 0) {
                  برائے (غیرعلامتی idx = 0 ;  idx < wid ;  idx += 1) {
			اگر (بنیاد+idx >= bits4_.size()) توڑو;
			bits4_.سیٹ_ثنائی_ہندسہ(بنیاد+idx, ثنائی_ہندسہ.value(idx));
		  }
		  needs_init_ = جھوٹ;
		  ptr.ptr()->send_vec4(bits4_,0);
	    } ورنہ {
		  منطقی بدلا = جھوٹ;
		  أكد(bits4_.size() == تفویض_نقاب.size());
		  برائے (غیرعلامتی idx = 0 ;  idx < wid ;  idx += 1) {
			اگر (بنیاد+idx >= bits4_.size()) توڑو;
			اگر (تفویض_نقاب.value(بنیاد+idx)) جاری;
			bits4_.سیٹ_ثنائی_ہندسہ(بنیاد+idx, ثنائی_ہندسہ.value(idx));
			بدلا = سچ;
		  }
		  اگر (بدلا) {
			needs_init_ = جھوٹ;
			ptr.ptr()->send_vec4(bits4_,0);
		  }
	    }
	    توڑو;

	  حالت 1: // Continuous assign value
	    اگر (تفویض_نقاب.size() == 0)
		  تفویض_نقاب = vvp_vector2_t(vvp_vector2_t::FILL0, bits4_.size());
	    برائے (غیرعلامتی idx = 0 ;  idx < wid ;  idx += 1) {
		  اگر (بنیاد+idx >= bits4_.size())
			توڑو;
		  bits4_.سیٹ_ثنائی_ہندسہ(بنیاد+idx, ثنائی_ہندسہ.value(idx));
		  تفویض_نقاب.سیٹ_ثنائی_ہندسہ(بنیاد+idx, 1);
	    }
	    ptr.ptr()->send_vec4(bits4_,0);
	    توڑو;

	  طےشدہ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.پورٹ());
	    أكد(0);
	    توڑو;
      }
}

خالی vvp_fun_signal4_sa::recv_vec8_pv(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&ثنائی_ہندسہ,
				      غیرعلامتی بنیاد, غیرعلامتی vwid)
{
      recv_vec4_pv(ptr, reduce4(ثنائی_ہندسہ), بنیاد, vwid, 0);
}

خالی vvp_fun_signal_بنیاد::deassign()
{
      continuous_تفویض_فعال = جھوٹ;
      تفویض_نقاب = vvp_vector2_t();
}

خالی vvp_fun_signal_بنیاد::deassign_pv(غیرعلامتی بنیاد, غیرعلامتی wid)
{
      برائے (غیرعلامتی idx = 0 ;  idx < wid ;  idx += 1) {
	    تفویض_نقاب.سیٹ_ثنائی_ہندسہ(بنیاد+idx, 0);
      }

      اگر (تفویض_نقاب.is_zero()) {
	    تفویض_نقاب = vvp_vector2_t();
      }
}

خالی automatic_signal_بنیاد::release(vvp_net_ptr_t,منطقی)
{
      أكد(0);
}

خالی automatic_signal_بنیاد::release_pv(vvp_net_ptr_t,غیرعلامتی,غیرعلامتی,منطقی)
{
      أكد(0);
}

غیرعلامتی automatic_signal_بنیاد::filter_سائز() ثابت
{
      أكد(0);
      واپس(0);
}
خالی automatic_signal_بنیاد::force_fil_vec4(ثابت vvp_vector4_t&, ثابت vvp_vector2_t&)
{
      أكد(0);
}
خالی automatic_signal_بنیاد::force_fil_vec8(ثابت vvp_vector8_t&, ثابت vvp_vector2_t&)
{
      أكد(0);
}
خالی automatic_signal_بنیاد::force_fil_real(دوہرا, ثابت vvp_vector2_t&)
{
      أكد(0);
}
خالی automatic_signal_بنیاد::get_قدر(ساخت t_vpi_قدر*)
{
      أكد(0);
}

ثابت vvp_vector4_t& vvp_fun_signal4_sa::vec4_unfiltered_قدر() ثابت
{
      واپس bits4_;
}

vvp_fun_signal4_aa::vvp_fun_signal4_aa(غیرعلامتی wid, vvp_bit4_t init)
{
      context_idx_ = vpip_جوڑو_شے_مقصد_context(یہ, vpip_peek_context_scope());
      سائز_2 = wid;
      init_ = init;
}

vvp_fun_signal4_aa::~vvp_fun_signal4_aa()
{
      أكد(0);
}

خالی vvp_fun_signal4_aa::alloc_instance(vvp_context_t context)
{
      vvp_سیٹ_context_شے(context, context_idx_, نیا vvp_vector4_t(سائز_2, init_));
}

خالی vvp_fun_signal4_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*بٹ = static_cast<vvp_vector4_t*>
            (vvp_get_context_شے(context, context_idx_));

      بٹ->fill_بٹ(init_);
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی vvp_fun_signal4_aa::free_instance(vvp_context_t context)
{
      vvp_vector4_t*بٹ = static_cast<vvp_vector4_t*>
            (vvp_get_context_شے(context, context_idx_));
      حذف بٹ;
}
#اگر_ختم

/*
 * Continuous and forced assignments are not permitted on automatic
 * variables. So we only expect to receive on port 0.
 */
خالی vvp_fun_signal4_aa::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                   vvp_context_t context)
{
      أكد(ptr.پورٹ() == 0);
      أكد(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_شے(context, context_idx_));

      اگر (!bits4->eeq(ثنائی_ہندسہ)) {
            *bits4 = ثنائی_ہندسہ;
            ptr.ptr()->send_vec4(*bits4, context);
      }
}

خالی vvp_fun_signal4_aa::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				      غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t context)
{
      أكد(ptr.پورٹ() == 0);
      أكد(سائز_2 == vwid);
      أكد(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_شے(context, context_idx_));

      غیرعلامتی wid = ثنائی_ہندسہ.size();
      برائے (غیرعلامتی idx = 0 ;  idx < wid ;  idx += 1) {
            اگر (بنیاد+idx >= bits4->size()) توڑو;
            bits4->سیٹ_ثنائی_ہندسہ(بنیاد+idx, ثنائی_ہندسہ.value(idx));
      }
      ptr.ptr()->send_vec4(*bits4, context);
}

غیرعلامتی vvp_fun_signal4_aa::قدر_سائز() ثابت
{
      واپس سائز_2;
}

vvp_bit4_t vvp_fun_signal4_aa::value(غیرعلامتی idx) ثابت
{
      ثابت vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_شے(context_idx_));

      واپس bits4->value(idx);
}

vvp_scalar_t vvp_fun_signal4_aa::scalar_قدر(غیرعلامتی idx) ثابت
{
      ثابت vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_شے(context_idx_));

      واپس vvp_scalar_t(bits4->value(idx), 6, 6);
}

خالی vvp_fun_signal4_aa::vec4_قدر(vvp_vector4_t&val) ثابت
{
      ثابت vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_شے(context_idx_));

      val = *bits4;
}

ثابت vvp_vector4_t&vvp_fun_signal4_aa::vec4_unfiltered_قدر() ثابت
{
      ثابت vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_شے(context_idx_));

      واپس *bits4;
}

خالی vvp_fun_signal4_aa::عامل حذف(خالی*)
{
      أكد(0);
}

/*
 * Testing for equality, we want a bitwise test instead of an
 * arithmetic test because we want to treat for example -0 different
 * from +0.
 */
منطقی بٹ_equal(دوہرا a, دوہرا b)
{
      واپس memcmp(&a, &b, حجم a) == 0;
}

vvp_fun_signal_real_sa::vvp_fun_signal_real_sa()
{
      بٹ_2 = 0.0;
}

دوہرا vvp_fun_signal_real_sa::real_unfiltered_قدر() ثابت
{
      واپس بٹ_2;
}

خالی vvp_fun_signal_real_sa::recv_real(vvp_net_ptr_t ptr, دوہرا ثنائی_ہندسہ,
                                       vvp_context_t)
{
      انتخاب (ptr.پورٹ()) {
	  حالت 0:
	    اگر (!continuous_تفویض_فعال) {
                  اگر (needs_init_ || !بٹ_equal(بٹ_2, ثنائی_ہندسہ)) {
			بٹ_2 = ثنائی_ہندسہ;
			needs_init_ = جھوٹ;
			ptr.ptr()->send_real(ثنائی_ہندسہ, 0);
		  }
	    }
	    توڑو;

	  حالت 1: // Continuous assign value
	    continuous_تفویض_فعال = سچ;
	    بٹ_2 = ثنائی_ہندسہ;
	    ptr.ptr()->send_real(ثنائی_ہندسہ, 0);
	    توڑو;

	  طےشدہ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.پورٹ());
	    أكد(0);
	    توڑو;
      }
}

vvp_fun_signal_real_aa::vvp_fun_signal_real_aa()
{
      context_idx_ = vpip_جوڑو_شے_مقصد_context(یہ, vpip_peek_context_scope());
}

vvp_fun_signal_real_aa::~vvp_fun_signal_real_aa()
{
      أكد(0);
}

خالی vvp_fun_signal_real_aa::alloc_instance(vvp_context_t context)
{
      دوہرا*بٹ = نیا دوہرا;
      vvp_سیٹ_context_شے(context, context_idx_, بٹ);

      *بٹ = 0.0;
}

خالی vvp_fun_signal_real_aa::reset_instance(vvp_context_t context)
{
      دوہرا*بٹ = static_cast<دوہرا*>
            (vvp_get_context_شے(context, context_idx_));

      *بٹ = 0.0;
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی vvp_fun_signal_real_aa::free_instance(vvp_context_t context)
{
      دوہرا*بٹ = static_cast<دوہرا*>
            (vvp_get_context_شے(context, context_idx_));
      حذف بٹ;
}
#اگر_ختم

دوہرا vvp_fun_signal_real_aa::real_unfiltered_قدر() ثابت
{
      ثابت دوہرا*بٹ = static_cast<دوہرا*>
            (vthread_get_rd_context_شے(context_idx_));

      واپس *بٹ;
}

دوہرا vvp_fun_signal_real_aa::real_قدر() ثابت
{
      واپس real_unfiltered_قدر();
}

خالی vvp_fun_signal_real_aa::recv_real(vvp_net_ptr_t ptr, دوہرا ثنائی_ہندسہ,
                                       vvp_context_t context)
{
      أكد(ptr.پورٹ() == 0);
      أكد(context);

      دوہرا*بٹ = static_cast<دوہرا*>
            (vvp_get_context_شے(context, context_idx_));

      اگر (!بٹ_equal(*بٹ,ثنائی_ہندسہ)) {
            *بٹ = ثنائی_ہندسہ;
            ptr.ptr()->send_real(ثنائی_ہندسہ, context);
      }
}

غیرعلامتی vvp_fun_signal_real_aa::قدر_سائز() ثابت
{
      أكد(0);
      واپس 1;
}

vvp_bit4_t vvp_fun_signal_real_aa::value(غیرعلامتی) ثابت
{
      أكد(0);
      واپس BIT4_X;
}

vvp_scalar_t vvp_fun_signal_real_aa::scalar_قدر(غیرعلامتی) ثابت
{
      أكد(0);
      واپس vvp_scalar_t();
}

خالی vvp_fun_signal_real_aa::vec4_قدر(vvp_vector4_t&) ثابت
{
      أكد(0);
}

خالی* vvp_fun_signal_real_aa::عامل نیا(std::size_t size)
{
      واپس vvp_net_fun_t::ڈھیر.alloc(size);
}

خالی vvp_fun_signal_real_aa::عامل حذف(خالی*)
{
      أكد(0);
}


vvp_fun_signal_string_sa::vvp_fun_signal_string_sa()
{
}

خالی vvp_fun_signal_string_sa::recv_string(vvp_net_ptr_t ptr, ثابت std::string&ثنائی_ہندسہ,
					   vvp_context_t)
{
      أكد(ptr.پورٹ() == 0);

      اگر (needs_init_ || قدر_2 != ثنائی_ہندسہ) {
	    قدر_2 = ثنائی_ہندسہ;
	    needs_init_ = جھوٹ;

	    ptr.ptr()->send_string(ثنائی_ہندسہ, 0);
      }
}

ثابت string& vvp_fun_signal_string_sa::get_string() ثابت
{
      واپس قدر_2;
}

vvp_fun_signal_string_aa::vvp_fun_signal_string_aa()
{
      context_idx_ = vpip_جوڑو_شے_مقصد_context(یہ, vpip_peek_context_scope());
}

vvp_fun_signal_string_aa::~vvp_fun_signal_string_aa()
{
      أكد(0);
}

خالی vvp_fun_signal_string_aa::alloc_instance(vvp_context_t context)
{
      string*بٹ = نیا std::string;
      vvp_سیٹ_context_شے(context, context_idx_, بٹ);
      *بٹ = "";
}

خالی vvp_fun_signal_string_aa::reset_instance(vvp_context_t context)
{
      string*بٹ = static_cast<std::string*>
	    (vvp_get_context_شے(context, context_idx_));
      *بٹ = "";
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی vvp_fun_signal_string_aa::free_instance(vvp_context_t context)
{
      string*بٹ = static_cast<std::string*>
            (vvp_get_context_شے(context, context_idx_));
      حذف بٹ;
}
#اگر_ختم

خالی vvp_fun_signal_string_aa::recv_string(vvp_net_ptr_t ptr, ثابت std::string&ثنائی_ہندسہ, vvp_context_t context)
{
      أكد(ptr.پورٹ() == 0);
      أكد(context);

      string*بٹ = static_cast<std::string*>
	    (vvp_get_context_شے(context, context_idx_));

      اگر (*بٹ != ثنائی_ہندسہ) {
	    *بٹ = ثنائی_ہندسہ;
	    ptr.ptr()->send_string(ثنائی_ہندسہ, context);
      }
}

غیرعلامتی vvp_fun_signal_string_aa::قدر_سائز() ثابت
{
      أكد(0);
      واپس 1;
}

vvp_bit4_t vvp_fun_signal_string_aa::value(غیرعلامتی) ثابت
{
      أكد(0);
      واپس BIT4_X;
}

vvp_scalar_t vvp_fun_signal_string_aa::scalar_قدر(غیرعلامتی) ثابت
{
      أكد(0);
      واپس vvp_scalar_t();
}

خالی vvp_fun_signal_string_aa::vec4_قدر(vvp_vector4_t&) ثابت
{
      أكد(0);
}

دوہرا vvp_fun_signal_string_aa::real_قدر() ثابت
{
      أكد(0);
      واپس 0.0;
}

ثابت std::string& vvp_fun_signal_string_aa::get_string() ثابت
{
      ثابت string*بٹ = static_cast<std::string*>
            (vthread_get_rd_context_شے(context_idx_));

      واپس *بٹ;
}

خالی* vvp_fun_signal_string_aa::عامل نیا(std::size_t size)
{
      واپس vvp_net_fun_t::ڈھیر.alloc(size);
}

خالی vvp_fun_signal_string_aa::عامل حذف(خالی*)
{
      أكد(0);
}

  /* OBJECT signals */

vvp_fun_signal_object_sa::vvp_fun_signal_object_sa(غیرعلامتی size)
: vvp_fun_signal_object(size)
{
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی vvp_fun_signal_object_aa::free_instance(vvp_context_t context)
{
      vvp_object_t*بٹ = static_cast<vvp_object_t*>
            (vvp_get_context_شے(context, context_idx_));
      حذف بٹ;
}
#اگر_ختم

خالی vvp_fun_signal_object_sa::recv_object(vvp_net_ptr_t ptr, vvp_object_t ثنائی_ہندسہ,
					   vvp_context_t)
{
      أكد(ptr.پورٹ() == 0);

      اگر (needs_init_ || قدر_2 != ثنائی_ہندسہ) {
	    قدر_2 = ثنائی_ہندسہ;
	    needs_init_ = جھوٹ;

	    ptr.ptr()->send_object(ثنائی_ہندسہ, 0);
      }
}

vvp_object_t vvp_fun_signal_object_sa::get_object() ثابت
{
      واپس قدر_2;
}

vvp_fun_signal_object_aa::vvp_fun_signal_object_aa(غیرعلامتی size)
: vvp_fun_signal_object(size)
{
      context_idx_ = vpip_جوڑو_شے_مقصد_context(یہ, vpip_peek_context_scope());
}

vvp_fun_signal_object_aa::~vvp_fun_signal_object_aa()
{
      أكد(0);
}

خالی vvp_fun_signal_object_aa::alloc_instance(vvp_context_t context)
{
      vvp_object_t*بٹ = نیا vvp_object_t;
      vvp_سیٹ_context_شے(context, context_idx_, بٹ);
      بٹ->reset();
}

خالی vvp_fun_signal_object_aa::reset_instance(vvp_context_t context)
{
      vvp_object_t*بٹ = static_cast<vvp_object_t*>
	    (vvp_get_context_شے(context, context_idx_));
      بٹ->reset();
}

vvp_object_t vvp_fun_signal_object_aa::get_object() ثابت
{
      ثابت vvp_object_t*بٹ = static_cast<vvp_object_t*>
	    (vthread_get_rd_context_شے(context_idx_));
      واپس *بٹ;
}

خالی vvp_fun_signal_object_aa::recv_object(vvp_net_ptr_t ptr, vvp_object_t ثنائی_ہندسہ,
					   vvp_context_t context)
{
      أكد(ptr.پورٹ() == 0);
      أكد(context);

      vvp_object_t*بٹ = static_cast<vvp_object_t*>
	    (vvp_get_context_شے(context, context_idx_));

      اگر (*بٹ != ثنائی_ہندسہ) {
	    *بٹ = ثنائی_ہندسہ;
	    ptr.ptr()->send_object(ثنائی_ہندسہ, context);
      }
}

غیرعلامتی vvp_fun_signal_object_aa::قدر_سائز() ثابت
{
      واپس 1;
}

vvp_bit4_t vvp_fun_signal_object_aa::value(غیرعلامتی) ثابت
{
      أكد(0);
      واپس BIT4_X;
}

vvp_scalar_t vvp_fun_signal_object_aa::scalar_قدر(غیرعلامتی) ثابت
{
      أكد(0);
      واپس vvp_scalar_t();
}

خالی vvp_fun_signal_object_aa::vec4_قدر(vvp_vector4_t&) ثابت
{
      أكد(0);
}

خالی* vvp_fun_signal_object_aa::عامل نیا(std::size_t size)
{
      واپس vvp_net_fun_t::ڈھیر.alloc(size);
}

خالی vvp_fun_signal_object_aa::عامل حذف(خالی*)
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

خالی vvp_fun_force::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			      vvp_context_t)
{
      أكد(ptr.پورٹ() == 0);
      vvp_net_t*net = ptr.ptr();

      vvp_net_t*dst = net->پورٹ[3].ptr();
      أكد(dst->fil);

      dst->force_vec4(coerce_مقصد_چوڑائی(ثنائی_ہندسہ, dst->fil->filter_سائز()), vvp_vector2_t(vvp_vector2_t::FILL1, dst->fil->filter_سائز()));
}

خالی vvp_fun_force::recv_real(vvp_net_ptr_t ptr, دوہرا ثنائی_ہندسہ, vvp_context_t)
{
      أكد(ptr.پورٹ() == 0);
      vvp_net_t*net = ptr.ptr();
      vvp_net_t*dst = net->پورٹ[3].ptr();
      dst->force_real(ثنائی_ہندسہ, vvp_vector2_t(vvp_vector2_t::FILL1, 1));
}

vvp_wire_بنیاد::vvp_wire_بنیاد()
{
}

vvp_wire_بنیاد::~vvp_wire_بنیاد()
{
}

vvp_bit4_t vvp_wire_بنیاد::driven_قدر(غیرعلامتی) ثابت
{
      أكد(0);
      واپس BIT4_X;
}

منطقی vvp_wire_بنیاد::is_forced(غیرعلامتی) ثابت
{
      أكد(0);
      واپس جھوٹ;
}

vvp_wire_vec4::vvp_wire_vec4(غیرعلامتی wid, vvp_bit4_t init)
: bits4_(wid, init)
{
      needs_init_ = سچ;
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec4(ثابت vvp_vector4_t&ثنائی_ہندسہ, vvp_vector4_t&rep,
						 غیرعلامتی بنیاد, غیرعلامتی vwid)
{
	// Special case! the input bit is 0 wid. Interpret this as a
	// vector of BIT4_X to match the width of the bits4_ vector.
	// FIXME! This is a hack to work around some buggy gate
	// implementations! This should be removed!
      اگر (بنیاد==0 && vwid==0) {
	    vvp_vector4_t tmp (bits4_.size(), BIT4_X);
	    اگر (bits4_ .eeq(tmp) && !needs_init_) واپس STOP;
	    bits4_ = tmp;
	    needs_init_ = جھوٹ;
	    واپس filter_نقاب(tmp, force4_, rep, 0);
      }

      اگر (vwid != bits4_.size()) {
	    cerr << "Internal error: Input vector expected width="
		 << bits4_.size() << ", got "
		 << "bit=" << ثنائی_ہندسہ << ", base=" << بنیاد << ", vwid=" << vwid
		 << سطر_ختم;
      }
      أكد(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      اگر (بنیاد==0 && ثنائی_ہندسہ.size()==vwid) {
	    اگر (bits4_ .eeq( ثنائی_ہندسہ ) && !needs_init_) واپس STOP;
	    bits4_ = ثنائی_ہندسہ;
      } ورنہ {
	    منطقی rc = bits4_.سیٹ_vec(بنیاد, ثنائی_ہندسہ);
	    اگر (rc == جھوٹ && !needs_init_) واپس STOP;
      }

      needs_init_ = جھوٹ;
      واپس filter_نقاب(ثنائی_ہندسہ, force4_, rep, بنیاد);
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec8(ثابت vvp_vector8_t&ثنائی_ہندسہ,
                                                 vvp_vector8_t&rep,
                                                 غیرعلامتی بنیاد,
                                                 غیرعلامتی vwid)
{
      أكد(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      vvp_vector4_t bit4 (reduce4(ثنائی_ہندسہ));
      اگر (بنیاد==0 && bit4.size()==vwid) {
	    اگر (bits4_ .eeq( bit4 ) && !needs_init_) واپس STOP;
	    bits4_ = bit4;
      } ورنہ {
	    منطقی rc = bits4_.سیٹ_vec(بنیاد, bit4);
	    اگر (rc == جھوٹ && !needs_init_) واپس STOP;
      }

      needs_init_ = جھوٹ;
      واپس filter_نقاب(ثنائی_ہندسہ, vvp_vector8_t(force4_,6,6), rep, بنیاد);
}

غیرعلامتی vvp_wire_vec4::filter_سائز() ثابت
{
      واپس bits4_.size();
}

خالی vvp_wire_vec4::force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&نقاب)
{
      force_نقاب(نقاب);

      اگر (force4_.size() == 0) {
	    force4_ = val;
      } ورنہ {
	    برائے (غیرعلامتی idx = 0; idx < نقاب.size() ; idx += 1) {
		  اگر (نقاب.value(idx) == 0)
			جاری;

		  force4_.سیٹ_ثنائی_ہندسہ(idx, val.value(idx));
	    }
      }
      تسلسل_vpi_callbacks();
}

خالی vvp_wire_vec4::force_fil_vec8(ثابت vvp_vector8_t&, ثابت vvp_vector2_t&)
{
      أكد(0);
}

خالی vvp_wire_vec4::force_fil_real(دوہرا, ثابت vvp_vector2_t&)
{
      أكد(0);
}

خالی vvp_wire_vec4::release(vvp_net_ptr_t ptr, منطقی net_flag)
{
      vvp_vector2_t نقاب (vvp_vector2_t::FILL1, bits4_.size());
      اگر (net_flag) {
	      // Wires revert to their unforced value after release.
            release_نقاب(نقاب);
	    needs_init_ = ! force4_ .eeq(bits4_);
	    ptr.ptr()->send_vec4(bits4_, 0);
	    تسلسل_vpi_callbacks();
      } ورنہ {
	      // Variables keep the current value.
	    vvp_vector4_t res (bits4_.size());
	    برائے (غیرعلامتی idx=0; idx<bits4_.size(); idx += 1)
		  res.سیٹ_ثنائی_ہندسہ(idx,value(idx));
            release_نقاب(نقاب);
	    ptr.ptr()->fun->recv_vec4(ptr, res, 0);
      }
}

خالی vvp_wire_vec4::release_pv(vvp_net_ptr_t ptr, غیرعلامتی بنیاد, غیرعلامتی wid, منطقی net_flag)
{
      أكد(bits4_.size() >= بنیاد + wid);

      vvp_vector2_t نقاب (vvp_vector2_t::FILL0, bits4_.size());
      برائے (غیرعلامتی idx = 0 ; idx < wid ; idx += 1)
	    نقاب.سیٹ_ثنائی_ہندسہ(بنیاد+idx, 1);

      اگر (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_نقاب(نقاب);
	    needs_init_ = ! force4_.subvalue(بنیاد,wid) .eeq(bits4_.subvalue(بنیاد,wid));
	    ptr.ptr()->send_vec4_pv(bits4_.subvalue(بنیاد,wid),
				    بنیاد, bits4_.size(), 0);
	    تسلسل_vpi_callbacks();
      } ورنہ {
	      // Variables keep the current value.
	    vvp_vector4_t res (wid);
	    برائے (غیرعلامتی idx=0; idx<wid; idx += 1)
		  res.سیٹ_ثنائی_ہندسہ(idx,value(بنیاد+idx));
	    release_نقاب(نقاب);
	    ptr.ptr()->fun->recv_vec4_pv(ptr, res, بنیاد, bits4_.size(), 0);
      }
}

غیرعلامتی vvp_wire_vec4::قدر_سائز() ثابت
{
      واپس bits4_.size();
}

vvp_bit4_t vvp_wire_vec4::filtered_قدر(غیرعلامتی idx) ثابت
{
      اگر (test_force_نقاب(idx))
	    واپس force4_.value(idx);
      ورنہ
	    واپس bits4_.value(idx);
}

vvp_bit4_t vvp_wire_vec4::value(غیرعلامتی idx) ثابت
{
      واپس filtered_قدر(idx);
}

vvp_scalar_t vvp_wire_vec4::scalar_قدر(غیرعلامتی idx) ثابت
{
      واپس vvp_scalar_t(value(idx),6,6);
}

خالی vvp_wire_vec4::vec4_قدر(vvp_vector4_t&val) ثابت
{
      val = bits4_;
      اگر (test_force_نقاب_is_zero())
	    واپس;

      برائے (غیرعلامتی idx = 0 ; idx < bits4_.size() ; idx += 1)
	    val.سیٹ_ثنائی_ہندسہ(idx, filtered_قدر(idx));
}

vvp_bit4_t vvp_wire_vec4::driven_قدر(غیرعلامتی idx) ثابت
{
      واپس bits4_.value(idx);
}

منطقی vvp_wire_vec4::is_forced(غیرعلامتی idx) ثابت
{
      واپس test_force_نقاب(idx);
}

vvp_wire_vec8::vvp_wire_vec8(غیرعلامتی wid)
: bits8_(wid)
{
      needs_init_ = سچ;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec4(ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                                 vvp_vector4_t&rep,
                                                 غیرعلامتی بنیاد,
                                                 غیرعلامتی vwid)
{
	// For now there is no support for a non-zero base.
      أكد(0 == بنیاد);
      أكد(bits8_.size() == vwid);
      أكد(bits8_.size() == ثنائی_ہندسہ.size());
	// QUESTION: Is it really correct to propagate a vec4 if this
	// is a vec8 node? In fact, it is really possible for a vec4
	// value to get through to a vec8 filter?
      vvp_vector8_t rep8;
      prop_t rc = filter_vec8(vvp_vector8_t(ثنائی_ہندسہ,6,6), rep8, 0, vwid);
      اگر (rc == REPL)
	    rep = reduce4(rep8);

      needs_init_ = جھوٹ;
      واپس rc;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec8(ثابت vvp_vector8_t&ثنائی_ہندسہ, vvp_vector8_t&rep, غیرعلامتی بنیاد, غیرعلامتی vwid)
{
      أكد(vwid == bits8_.size());
	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      اگر (بنیاد==0 && ثنائی_ہندسہ.size()==vwid) {
	    اگر (bits8_.eeq(ثنائی_ہندسہ) && !needs_init_) واپس STOP;
	    bits8_ = ثنائی_ہندسہ;
      } ورنہ {
	    اگر (bits8_.subvalue(بنیاد, ثنائی_ہندسہ.size()).eeq(ثنائی_ہندسہ) &&
		!needs_init_) {
		  واپس STOP;
	    }
	    اگر (bits8_.size() == 0)
		  bits8_ = vvp_vector8_t(vwid);
	    أكد(bits8_.size() == vwid);
	    bits8_.سیٹ_vec(بنیاد, ثنائی_ہندسہ);
      }
      needs_init_ = جھوٹ;
      واپس filter_نقاب(ثنائی_ہندسہ, force8_, rep, بنیاد);
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_داخلہ_vec8(ثابت vvp_vector8_t&ثنائی_ہندسہ, vvp_vector8_t&rep) ثابت
{
      واپس filter_داخلہ_نقاب(ثنائی_ہندسہ, force8_, rep);
}

غیرعلامتی vvp_wire_vec8::filter_سائز() ثابت
{
      واپس bits8_.size();
}

خالی vvp_wire_vec8::force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&نقاب)
{
      force_fil_vec8(vvp_vector8_t(val,6,6), نقاب);
}

خالی vvp_wire_vec8::force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&نقاب)
{
      force_نقاب(نقاب);

      اگر (force8_.size() == 0) {
	    force8_ = val;
      } ورنہ {
	    برائے (غیرعلامتی idx = 0; idx < نقاب.size() ; idx += 1) {
		  اگر (نقاب.value(idx) == 0)
			جاری;

		  force8_.سیٹ_ثنائی_ہندسہ(idx, val.value(idx));
	    }
      }
      تسلسل_vpi_callbacks();
}

خالی vvp_wire_vec8::force_fil_real(دوہرا, ثابت vvp_vector2_t&)
{
      أكد(0);
}

خالی vvp_wire_vec8::release(vvp_net_ptr_t ptr, منطقی net_flag)
{
	// Wires revert to their unforced value after release.
      vvp_vector2_t نقاب (vvp_vector2_t::FILL1, bits8_.size());
      release_نقاب(نقاب);
      اگر (net_flag) {
	    needs_init_ = !force8_ .eeq(bits8_);
	    ptr.ptr()->send_vec8(bits8_);
      } ورنہ {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    أكد(0);
//	    ptr.ptr()->fun->recv_vec8(ptr, force8_);
      }
}

خالی vvp_wire_vec8::release_pv(vvp_net_ptr_t ptr, غیرعلامتی بنیاد, غیرعلامتی wid, منطقی net_flag)
{
      أكد(bits8_.size() >= بنیاد + wid);

      vvp_vector2_t نقاب (vvp_vector2_t::FILL0, bits8_.size());
      برائے (غیرعلامتی idx = 0 ; idx < wid ; idx += 1)
	    نقاب.سیٹ_ثنائی_ہندسہ(بنیاد+idx, 1);

      release_نقاب(نقاب);

      اگر (net_flag) {
	    needs_init_ = !force8_.subvalue(بنیاد,wid) .eeq((bits8_.subvalue(بنیاد,wid)));
	    ptr.ptr()->send_vec8_pv(bits8_.subvalue(بنیاد,wid),
				    بنیاد, bits8_.size());
	    تسلسل_vpi_callbacks();
      } ورنہ {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    أكد(0);
//	    ptr.ptr()->fun->recv_vec8_pv(ptr, force8_.subvalue(base,wid),
//					 base, force8_.size());
      }
}

غیرعلامتی vvp_wire_vec8::قدر_سائز() ثابت
{
      واپس bits8_.size();
}

vvp_scalar_t vvp_wire_vec8::filtered_قدر(غیرعلامتی idx) ثابت
{
      اگر (test_force_نقاب(idx))
	    واپس force8_.value(idx);
      ورنہ
	    واپس bits8_.value(idx);
}

vvp_bit4_t vvp_wire_vec8::value(غیرعلامتی idx) ثابت
{
      واپس filtered_قدر(idx).value();
}

vvp_scalar_t vvp_wire_vec8::scalar_قدر(غیرعلامتی idx) ثابت
{
      واپس filtered_قدر(idx);
}

vvp_vector8_t vvp_wire_vec8::vec8_قدر() ثابت
{
      vvp_vector8_t tmp = bits8_;
      برائے (غیرعلامتی idx = 0 ; idx < bits8_.size() ; idx += 1)
	    tmp.سیٹ_ثنائی_ہندسہ(idx, filtered_قدر(idx));
      واپس tmp;
}

خالی vvp_wire_vec8::vec4_قدر(vvp_vector4_t&val) ثابت
{
      val = reduce4(vec8_قدر());
}

vvp_bit4_t vvp_wire_vec8::driven_قدر(غیرعلامتی idx) ثابت
{
      واپس bits8_.value(idx).value();
}

منطقی vvp_wire_vec8::is_forced(غیرعلامتی idx) ثابت
{
      واپس test_force_نقاب(idx);
}

vvp_wire_real::vvp_wire_real()
: ثنائی_ہندسہ_2(0.0), force_(0.0)
{
}

vvp_net_fil_t::prop_t vvp_wire_real::filter_real(دوہرا&ثنائی_ہندسہ)
{
      اگر (بٹ_equal(ثنائی_ہندسہ_2, ثنائی_ہندسہ) && !needs_init_) واپس STOP;
      ثنائی_ہندسہ_2 = ثنائی_ہندسہ;
      needs_init_ = جھوٹ;
      واپس filter_نقاب(ثنائی_ہندسہ, force_);
}

غیرعلامتی vvp_wire_real::filter_سائز() ثابت
{
      أكد(0);
      واپس 0;
}

خالی vvp_wire_real::force_fil_vec4(ثابت vvp_vector4_t&, ثابت vvp_vector2_t&)
{
      أكد(0);
}

خالی vvp_wire_real::force_fil_vec8(ثابت vvp_vector8_t&, ثابت vvp_vector2_t&)
{
      أكد(0);
}

خالی vvp_wire_real::force_fil_real(دوہرا val, ثابت vvp_vector2_t&نقاب)
{
      force_نقاب(نقاب);
      اگر (نقاب.value(0))
	    force_ = val;

      تسلسل_vpi_callbacks();
}

خالی vvp_wire_real::release(vvp_net_ptr_t ptr, منطقی net_flag)
{
      vvp_vector2_t نقاب (vvp_vector2_t::FILL1, 1);
      اگر (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_نقاب(نقاب);
	    needs_init_ = !بٹ_equal(force_, ثنائی_ہندسہ_2);
	    ptr.ptr()->send_real(ثنائی_ہندسہ_2, 0);
      } ورنہ {
	      // Variables keep the current value.
	    دوہرا res =  real_قدر();
	    release_نقاب(نقاب);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
}

خالی vvp_wire_real::release_pv(vvp_net_ptr_t, غیرعلامتی, غیرعلامتی, منطقی)
{
      أكد(0);
#اگر 0
	// A real is a single value. If for some reason this part release
	// can happen the following code should work correctly (requires
	// a base of 0 and a width of 1).
      vvp_vector2_t نقاب (vvp_vector2_t::FILL1, 1);
      أكد(بنیاد == 0 && wid == 1);

      اگر (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_نقاب(نقاب);
	    ptr.ptr()->send_real(ثنائی_ہندسہ_2, 0);
      } ورنہ {
	      // Variables keep the current value.
	    دوہرا res =  real_قدر();
	    release_نقاب(نقاب);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
#اگر_ختم
}

غیرعلامتی vvp_wire_real::قدر_سائز() ثابت
{
      أكد(0);
      واپس 1;
}

vvp_bit4_t vvp_wire_real::value(غیرعلامتی) ثابت
{
      أكد(0);
      واپس BIT4_X;
}

vvp_scalar_t vvp_wire_real::scalar_قدر(غیرعلامتی) ثابت
{
      أكد(0);
      واپس vvp_scalar_t();
}

خالی vvp_wire_real::vec4_قدر(vvp_vector4_t&) ثابت
{
      أكد(0);
}

دوہرا vvp_wire_real::real_قدر() ثابت
{
      اگر (test_force_نقاب(0))
	    واپس force_;
      ورنہ
	    واپس ثنائی_ہندسہ_2;
}

#اگر 0
vvp_wire_string::vvp_wire_string()
{
}

غیرعلامتی vvp_wire_string::filter_سائز() ثابت
{
      أكد(0);
      واپس 0;
}

خالی vvp_wire_string::force_fil_vec4(ثابت vvp_vector4_t&, ثابت vvp_vector2_t&)
{
      أكد(0);
}
خالی vvp_wire_string::force_fil_vec8(ثابت vvp_vector8_t&, ثابت vvp_vector2_t&)
{
      أكد(0);
}
خالی vvp_wire_string::force_fil_real(دوہرا, ثابت vvp_vector2_t&)
{
      أكد(0);
}

خالی vvp_wire_string::release(vvp_net_ptr_t ptr, منطقی net_flag)
{
      أكد(0);
}

خالی vvp_wire_string::release_pv(vvp_net_ptr_t, غیرعلامتی, غیرعلامتی, منطقی)
{
      أكد(0);
}

غیرعلامتی vvp_wire_string::قدر_سائز() ثابت
{
      أكد(0);
      واپس 1;
}

vvp_bit4_t vvp_wire_string::value(غیرعلامتی) ثابت
{
      أكد(0);
      واپس BIT4_X;
}

vvp_scalar_t vvp_wire_string::scalar_قدر(غیرعلامتی) ثابت
{
      أكد(0);
      واپس vvp_scalar_t();
}

خالی vvp_wire_string::vec4_قدر(vvp_vector4_t&) ثابت
{
      أكد(0);
}

دوہرا vvp_wire_string::real_قدر() ثابت
{
      أكد(0);
      واپس 0.0;
}
#اگر_ختم
