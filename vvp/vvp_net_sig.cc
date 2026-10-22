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
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
# include  <map>
#यदि_अन्तः

# include  <iostream>

उपयुज्य नामाकाश std;

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
प्रतिरूपम् <वर्ग T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_आवरणम्(ध्रुव T&val, ध्रुव T&force, T&filter, अचिह्नित आधारः)
{
      यदि (!test_force_आवरणम्_is_zero()) {
	      // Some bits are being forced. Go through the
	      // force_mask_ and force value to see which bits are
	      // propagated and which are kept from the forced
	      // value. Update the filter with the filtered result and
	      // return REPL to indicate that some bits have changed,
	      // or STOP if no bits change.
	    तार्किक propagate_flag = force_propagate_;
	    force_propagate_ = असत्य;
	    प्रतिज्ञा(force_आवरणम्_2.size() == force.size());
	    प्रतिज्ञा((आधारः+val.size()) <= force_आवरणम्_2.size());

	    filter = val;
	    प्रति (अचिह्नित idx = 0 ; idx < val.size() ; idx += 1) {
		  यदि (force_आवरणम्_2.value(आधारः+idx))
			filter.समुच्चयः_bit(idx, force.value(आधारः+idx));
		  अन्यथा
			propagate_flag = सत्य;
	    }

	    यदि (propagate_flag) {
		  सततम्_vpi_callbacks();
		  प्रत्यागम REPL;
	    } अन्यथा {
		  प्रत्यागम STOP;
	    }

      } अन्यथा {
	    सततम्_vpi_callbacks();
	    प्रत्यागम PROP;
      }
}

प्रतिरूपम् <वर्ग T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_आवरणम्(T&val, T force)
{

      यदि (test_force_आवरणम्(0)) {
	    val = force;
	    सततम्_vpi_callbacks();
	    प्रत्यागम REPL;
      }
      सततम्_vpi_callbacks();
      प्रत्यागम PROP;
}

प्रतिरूपम् <वर्ग T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_निवेशः_आवरणम्(ध्रुव T&val, ध्रुव T&force, T&rep) ध्रुव
{
      यदि (test_force_आवरणम्_is_zero())
	    प्रत्यागम PROP;

      प्रतिज्ञा(force_आवरणम्_2.size() == force.size());

      rep = val;
      प्रति (अचिह्नित idx = 0 ; idx < val.size() ; idx += 1) {
	    यदि (force_आवरणम्_2.value(idx))
		  rep.समुच्चयः_bit(idx, force.value(idx));
      }

      प्रत्यागम REPL;
}

vvp_signal_मानम्::~vvp_signal_मानम्()
{
}

द्विगुण vvp_signal_मानम्::real_मानम्() ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम 0;
}

शून्य vvp_net_t::force_vec4(ध्रुव vvp_vector4_t&val, ध्रुव vvp_vector2_t&आवरणम्)
{
      प्रतिज्ञा(fil);
      fil->force_fil_vec4(val, आवरणम्);
      fun->force_flag(असत्य);
      vvp_send_vec4(बहिर्गमनम्_2, val, 0);
}

शून्य vvp_net_t::force_vec8(ध्रुव vvp_vector8_t&val, ध्रुव vvp_vector2_t&आवरणम्)
{
      प्रतिज्ञा(fil);
      fil->force_fil_vec8(val, आवरणम्);
      fun->force_flag(असत्य);
      vvp_send_vec8(बहिर्गमनम्_2, val);
}

शून्य vvp_net_t::force_real(द्विगुण val, ध्रुव vvp_vector2_t&आवरणम्)
{
      प्रतिज्ञा(fil);
      fil->force_fil_real(val, आवरणम्);
      fun->force_flag(असत्य);
      vvp_send_real(बहिर्गमनम्_2, val, 0);
}

/* **** vvp_fun_signal methods **** */

vvp_fun_signal_आधारः::vvp_fun_signal_आधारः()
{
      continuous_नियोजय_सक्रिय = असत्य;
      needs_init_ = सत्य;
      cassign_link = 0;
      गणनम्_functors_sig += 1;
}

vvp_fun_signal4_sa::vvp_fun_signal4_sa(अचिह्नित wid, vvp_bit4_t init)
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
शून्य vvp_fun_signal4_sa::recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                                   vvp_context_t)
{
      विकल्प (ptr.पत्तनम्()) {
	  पक्ष 0: // Normal input (feed from net, or set from process)
	      /* If we don't have a continuous assign mask then just
		 copy the bits, otherwise we need to see if there are
		 any holes in the mask so we can set those bits. */
	    यदि (नियोजय_आवरणम्.size() == 0) {
                  यदि (needs_init_ || !bits4_.eeq(bit)) {
			प्रतिज्ञा(bit.size() == bits4_.size());
			bits4_ = bit;
			needs_init_ = असत्य;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    } अन्यथा {
		  तार्किक परिवर्तितम् = असत्य;
		  प्रतिज्ञा(bits4_.size() == नियोजय_आवरणम्.size());
		  प्रति (अचिह्नित idx = 0 ;  idx < bit.size() ;  idx += 1) {
			यदि (idx >= bits4_.size()) भङ्ग;
			यदि (नियोजय_आवरणम्.value(idx)) अनुवर्त;
			bits4_.समुच्चयः_bit(idx, bit.value(idx));
			परिवर्तितम् = सत्य;
		  }
		  यदि (परिवर्तितम्) {
			needs_init_ = असत्य;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    }
	    भङ्ग;

	  पक्ष 1: // Continuous assign value
	      // Handle the simple case of the linked source being wider
	      // than this signal. Note we don't yet support the case of
	      // the linked source being narrower than this signal, or
	      // the case of an expression being assigned.
	    bits4_ = coerce_गन्तव्यम्_विस्तारः(bit, bits4_.size());
	    नियोजय_आवरणम् = vvp_vector2_t(vvp_vector2_t::FILL1, bits4_.size());
	    ptr.ptr()->send_vec4(bits4_, 0);
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.पत्तनम्());
	    प्रतिज्ञा(0);
	    भङ्ग;
      }
}

शून्य vvp_fun_signal4_sa::recv_vec8(vvp_net_ptr_t ptr, ध्रुव vvp_vector8_t&bit)
{
      recv_vec4(ptr, reduce4(bit), 0);
}

शून्य vvp_fun_signal4_sa::recv_vec4_pv(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
				      अचिह्नित आधारः, अचिह्नित vwid, vvp_context_t)
{
      प्रतिज्ञा(bits4_.size() == vwid);
      अचिह्नित wid = bit.size();

      विकल्प (ptr.पत्तनम्()) {
	  पक्ष 0: // Normal input
	    यदि (नियोजय_आवरणम्.size() == 0) {
                  प्रति (अचिह्नित idx = 0 ;  idx < wid ;  idx += 1) {
			यदि (आधारः+idx >= bits4_.size()) भङ्ग;
			bits4_.समुच्चयः_bit(आधारः+idx, bit.value(idx));
		  }
		  needs_init_ = असत्य;
		  ptr.ptr()->send_vec4(bits4_,0);
	    } अन्यथा {
		  तार्किक परिवर्तितम् = असत्य;
		  प्रतिज्ञा(bits4_.size() == नियोजय_आवरणम्.size());
		  प्रति (अचिह्नित idx = 0 ;  idx < wid ;  idx += 1) {
			यदि (आधारः+idx >= bits4_.size()) भङ्ग;
			यदि (नियोजय_आवरणम्.value(आधारः+idx)) अनुवर्त;
			bits4_.समुच्चयः_bit(आधारः+idx, bit.value(idx));
			परिवर्तितम् = सत्य;
		  }
		  यदि (परिवर्तितम्) {
			needs_init_ = असत्य;
			ptr.ptr()->send_vec4(bits4_,0);
		  }
	    }
	    भङ्ग;

	  पक्ष 1: // Continuous assign value
	    यदि (नियोजय_आवरणम्.size() == 0)
		  नियोजय_आवरणम् = vvp_vector2_t(vvp_vector2_t::FILL0, bits4_.size());
	    प्रति (अचिह्नित idx = 0 ;  idx < wid ;  idx += 1) {
		  यदि (आधारः+idx >= bits4_.size())
			भङ्ग;
		  bits4_.समुच्चयः_bit(आधारः+idx, bit.value(idx));
		  नियोजय_आवरणम्.समुच्चयः_bit(आधारः+idx, 1);
	    }
	    ptr.ptr()->send_vec4(bits4_,0);
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.पत्तनम्());
	    प्रतिज्ञा(0);
	    भङ्ग;
      }
}

शून्य vvp_fun_signal4_sa::recv_vec8_pv(vvp_net_ptr_t ptr, ध्रुव vvp_vector8_t&bit,
				      अचिह्नित आधारः, अचिह्नित vwid)
{
      recv_vec4_pv(ptr, reduce4(bit), आधारः, vwid, 0);
}

शून्य vvp_fun_signal_आधारः::deassign()
{
      continuous_नियोजय_सक्रिय = असत्य;
      नियोजय_आवरणम् = vvp_vector2_t();
}

शून्य vvp_fun_signal_आधारः::deassign_pv(अचिह्नित आधारः, अचिह्नित wid)
{
      प्रति (अचिह्नित idx = 0 ;  idx < wid ;  idx += 1) {
	    नियोजय_आवरणम्.समुच्चयः_bit(आधारः+idx, 0);
      }

      यदि (नियोजय_आवरणम्.is_zero()) {
	    नियोजय_आवरणम् = vvp_vector2_t();
      }
}

शून्य automatic_signal_आधारः::release(vvp_net_ptr_t,तार्किक)
{
      प्रतिज्ञा(0);
}

शून्य automatic_signal_आधारः::release_pv(vvp_net_ptr_t,अचिह्नित,अचिह्नित,तार्किक)
{
      प्रतिज्ञा(0);
}

अचिह्नित automatic_signal_आधारः::filter_आकार() ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम(0);
}
शून्य automatic_signal_आधारः::force_fil_vec4(ध्रुव vvp_vector4_t&, ध्रुव vvp_vector2_t&)
{
      प्रतिज्ञा(0);
}
शून्य automatic_signal_आधारः::force_fil_vec8(ध्रुव vvp_vector8_t&, ध्रुव vvp_vector2_t&)
{
      प्रतिज्ञा(0);
}
शून्य automatic_signal_आधारः::force_fil_real(द्विगुण, ध्रुव vvp_vector2_t&)
{
      प्रतिज्ञा(0);
}
शून्य automatic_signal_आधारः::get_मानम्(संरचना t_vpi_मानम्*)
{
      प्रतिज्ञा(0);
}

ध्रुव vvp_vector4_t& vvp_fun_signal4_sa::vec4_unfiltered_मानम्() ध्रुव
{
      प्रत्यागम bits4_;
}

vvp_fun_signal4_aa::vvp_fun_signal4_aa(अचिह्नित wid, vvp_bit4_t init)
{
      context_idx_ = vpip_सङ्कलय_item_गन्तव्यम्_context(एतत्, vpip_peek_context_scope());
      आकार_2 = wid;
      init_ = init;
}

vvp_fun_signal4_aa::~vvp_fun_signal4_aa()
{
      प्रतिज्ञा(0);
}

शून्य vvp_fun_signal4_aa::alloc_instance(vvp_context_t context)
{
      vvp_समुच्चयः_context_item(context, context_idx_, नूतन vvp_vector4_t(आकार_2, init_));
}

शून्य vvp_fun_signal4_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*द्वयङ्काः = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      द्वयङ्काः->fill_द्वयङ्काः(init_);
}

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
शून्य vvp_fun_signal4_aa::free_instance(vvp_context_t context)
{
      vvp_vector4_t*द्वयङ्काः = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));
      विलोपय द्वयङ्काः;
}
#यदि_अन्तः

/*
 * Continuous and forced assignments are not permitted on automatic
 * variables. So we only expect to receive on port 0.
 */
शून्य vvp_fun_signal4_aa::recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      प्रतिज्ञा(ptr.पत्तनम्() == 0);
      प्रतिज्ञा(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      यदि (!bits4->eeq(bit)) {
            *bits4 = bit;
            ptr.ptr()->send_vec4(*bits4, context);
      }
}

शून्य vvp_fun_signal4_aa::recv_vec4_pv(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
				      अचिह्नित आधारः, अचिह्नित vwid, vvp_context_t context)
{
      प्रतिज्ञा(ptr.पत्तनम्() == 0);
      प्रतिज्ञा(आकार_2 == vwid);
      प्रतिज्ञा(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      अचिह्नित wid = bit.size();
      प्रति (अचिह्नित idx = 0 ;  idx < wid ;  idx += 1) {
            यदि (आधारः+idx >= bits4->size()) भङ्ग;
            bits4->समुच्चयः_bit(आधारः+idx, bit.value(idx));
      }
      ptr.ptr()->send_vec4(*bits4, context);
}

अचिह्नित vvp_fun_signal4_aa::मानम्_आकार() ध्रुव
{
      प्रत्यागम आकार_2;
}

vvp_bit4_t vvp_fun_signal4_aa::value(अचिह्नित idx) ध्रुव
{
      ध्रुव vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      प्रत्यागम bits4->value(idx);
}

vvp_scalar_t vvp_fun_signal4_aa::scalar_मानम्(अचिह्नित idx) ध्रुव
{
      ध्रुव vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      प्रत्यागम vvp_scalar_t(bits4->value(idx), 6, 6);
}

शून्य vvp_fun_signal4_aa::vec4_मानम्(vvp_vector4_t&val) ध्रुव
{
      ध्रुव vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      val = *bits4;
}

ध्रुव vvp_vector4_t&vvp_fun_signal4_aa::vec4_unfiltered_मानम्() ध्रुव
{
      ध्रुव vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      प्रत्यागम *bits4;
}

शून्य vvp_fun_signal4_aa::संक्रियकः विलोपय(शून्य*)
{
      प्रतिज्ञा(0);
}

/*
 * Testing for equality, we want a bitwise test instead of an
 * arithmetic test because we want to treat for example -0 different
 * from +0.
 */
तार्किक द्वयङ्काः_equal(द्विगुण a, द्विगुण b)
{
      प्रत्यागम memcmp(&a, &b, परिमाण a) == 0;
}

vvp_fun_signal_real_sa::vvp_fun_signal_real_sa()
{
      द्वयङ्काः_2 = 0.0;
}

द्विगुण vvp_fun_signal_real_sa::real_unfiltered_मानम्() ध्रुव
{
      प्रत्यागम द्वयङ्काः_2;
}

शून्य vvp_fun_signal_real_sa::recv_real(vvp_net_ptr_t ptr, द्विगुण bit,
                                       vvp_context_t)
{
      विकल्प (ptr.पत्तनम्()) {
	  पक्ष 0:
	    यदि (!continuous_नियोजय_सक्रिय) {
                  यदि (needs_init_ || !द्वयङ्काः_equal(द्वयङ्काः_2, bit)) {
			द्वयङ्काः_2 = bit;
			needs_init_ = असत्य;
			ptr.ptr()->send_real(bit, 0);
		  }
	    }
	    भङ्ग;

	  पक्ष 1: // Continuous assign value
	    continuous_नियोजय_सक्रिय = सत्य;
	    द्वयङ्काः_2 = bit;
	    ptr.ptr()->send_real(bit, 0);
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.पत्तनम्());
	    प्रतिज्ञा(0);
	    भङ्ग;
      }
}

vvp_fun_signal_real_aa::vvp_fun_signal_real_aa()
{
      context_idx_ = vpip_सङ्कलय_item_गन्तव्यम्_context(एतत्, vpip_peek_context_scope());
}

vvp_fun_signal_real_aa::~vvp_fun_signal_real_aa()
{
      प्रतिज्ञा(0);
}

शून्य vvp_fun_signal_real_aa::alloc_instance(vvp_context_t context)
{
      द्विगुण*द्वयङ्काः = नूतन द्विगुण;
      vvp_समुच्चयः_context_item(context, context_idx_, द्वयङ्काः);

      *द्वयङ्काः = 0.0;
}

शून्य vvp_fun_signal_real_aa::reset_instance(vvp_context_t context)
{
      द्विगुण*द्वयङ्काः = static_cast<द्विगुण*>
            (vvp_get_context_item(context, context_idx_));

      *द्वयङ्काः = 0.0;
}

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
शून्य vvp_fun_signal_real_aa::free_instance(vvp_context_t context)
{
      द्विगुण*द्वयङ्काः = static_cast<द्विगुण*>
            (vvp_get_context_item(context, context_idx_));
      विलोपय द्वयङ्काः;
}
#यदि_अन्तः

द्विगुण vvp_fun_signal_real_aa::real_unfiltered_मानम्() ध्रुव
{
      ध्रुव द्विगुण*द्वयङ्काः = static_cast<द्विगुण*>
            (vthread_get_rd_context_item(context_idx_));

      प्रत्यागम *द्वयङ्काः;
}

द्विगुण vvp_fun_signal_real_aa::real_मानम्() ध्रुव
{
      प्रत्यागम real_unfiltered_मानम्();
}

शून्य vvp_fun_signal_real_aa::recv_real(vvp_net_ptr_t ptr, द्विगुण bit,
                                       vvp_context_t context)
{
      प्रतिज्ञा(ptr.पत्तनम्() == 0);
      प्रतिज्ञा(context);

      द्विगुण*द्वयङ्काः = static_cast<द्विगुण*>
            (vvp_get_context_item(context, context_idx_));

      यदि (!द्वयङ्काः_equal(*द्वयङ्काः,bit)) {
            *द्वयङ्काः = bit;
            ptr.ptr()->send_real(bit, context);
      }
}

अचिह्नित vvp_fun_signal_real_aa::मानम्_आकार() ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम 1;
}

vvp_bit4_t vvp_fun_signal_real_aa::value(अचिह्नित) ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम BIT4_X;
}

vvp_scalar_t vvp_fun_signal_real_aa::scalar_मानम्(अचिह्नित) ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम vvp_scalar_t();
}

शून्य vvp_fun_signal_real_aa::vec4_मानम्(vvp_vector4_t&) ध्रुव
{
      प्रतिज्ञा(0);
}

शून्य* vvp_fun_signal_real_aa::संक्रियकः नूतन(std::size_t size)
{
      प्रत्यागम vvp_net_fun_t::heap_.alloc(size);
}

शून्य vvp_fun_signal_real_aa::संक्रियकः विलोपय(शून्य*)
{
      प्रतिज्ञा(0);
}


vvp_fun_signal_string_sa::vvp_fun_signal_string_sa()
{
}

शून्य vvp_fun_signal_string_sa::recv_string(vvp_net_ptr_t ptr, ध्रुव std::string&bit,
					   vvp_context_t)
{
      प्रतिज्ञा(ptr.पत्तनम्() == 0);

      यदि (needs_init_ || मानम्_2 != bit) {
	    मानम्_2 = bit;
	    needs_init_ = असत्य;

	    ptr.ptr()->send_string(bit, 0);
      }
}

ध्रुव string& vvp_fun_signal_string_sa::get_string() ध्रुव
{
      प्रत्यागम मानम्_2;
}

vvp_fun_signal_string_aa::vvp_fun_signal_string_aa()
{
      context_idx_ = vpip_सङ्कलय_item_गन्तव्यम्_context(एतत्, vpip_peek_context_scope());
}

vvp_fun_signal_string_aa::~vvp_fun_signal_string_aa()
{
      प्रतिज्ञा(0);
}

शून्य vvp_fun_signal_string_aa::alloc_instance(vvp_context_t context)
{
      string*द्वयङ्काः = नूतन std::string;
      vvp_समुच्चयः_context_item(context, context_idx_, द्वयङ्काः);
      *द्वयङ्काः = "";
}

शून्य vvp_fun_signal_string_aa::reset_instance(vvp_context_t context)
{
      string*द्वयङ्काः = static_cast<std::string*>
	    (vvp_get_context_item(context, context_idx_));
      *द्वयङ्काः = "";
}

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
शून्य vvp_fun_signal_string_aa::free_instance(vvp_context_t context)
{
      string*द्वयङ्काः = static_cast<std::string*>
            (vvp_get_context_item(context, context_idx_));
      विलोपय द्वयङ्काः;
}
#यदि_अन्तः

शून्य vvp_fun_signal_string_aa::recv_string(vvp_net_ptr_t ptr, ध्रुव std::string&bit, vvp_context_t context)
{
      प्रतिज्ञा(ptr.पत्तनम्() == 0);
      प्रतिज्ञा(context);

      string*द्वयङ्काः = static_cast<std::string*>
	    (vvp_get_context_item(context, context_idx_));

      यदि (*द्वयङ्काः != bit) {
	    *द्वयङ्काः = bit;
	    ptr.ptr()->send_string(bit, context);
      }
}

अचिह्नित vvp_fun_signal_string_aa::मानम्_आकार() ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम 1;
}

vvp_bit4_t vvp_fun_signal_string_aa::value(अचिह्नित) ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम BIT4_X;
}

vvp_scalar_t vvp_fun_signal_string_aa::scalar_मानम्(अचिह्नित) ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम vvp_scalar_t();
}

शून्य vvp_fun_signal_string_aa::vec4_मानम्(vvp_vector4_t&) ध्रुव
{
      प्रतिज्ञा(0);
}

द्विगुण vvp_fun_signal_string_aa::real_मानम्() ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम 0.0;
}

ध्रुव std::string& vvp_fun_signal_string_aa::get_string() ध्रुव
{
      ध्रुव string*द्वयङ्काः = static_cast<std::string*>
            (vthread_get_rd_context_item(context_idx_));

      प्रत्यागम *द्वयङ्काः;
}

शून्य* vvp_fun_signal_string_aa::संक्रियकः नूतन(std::size_t size)
{
      प्रत्यागम vvp_net_fun_t::heap_.alloc(size);
}

शून्य vvp_fun_signal_string_aa::संक्रियकः विलोपय(शून्य*)
{
      प्रतिज्ञा(0);
}

  /* OBJECT signals */

vvp_fun_signal_object_sa::vvp_fun_signal_object_sa(अचिह्नित size)
: vvp_fun_signal_object(size)
{
}

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
शून्य vvp_fun_signal_object_aa::free_instance(vvp_context_t context)
{
      vvp_object_t*द्वयङ्काः = static_cast<vvp_object_t*>
            (vvp_get_context_item(context, context_idx_));
      विलोपय द्वयङ्काः;
}
#यदि_अन्तः

शून्य vvp_fun_signal_object_sa::recv_object(vvp_net_ptr_t ptr, vvp_object_t bit,
					   vvp_context_t)
{
      प्रतिज्ञा(ptr.पत्तनम्() == 0);

      यदि (needs_init_ || मानम्_2 != bit) {
	    मानम्_2 = bit;
	    needs_init_ = असत्य;

	    ptr.ptr()->send_object(bit, 0);
      }
}

vvp_object_t vvp_fun_signal_object_sa::get_object() ध्रुव
{
      प्रत्यागम मानम्_2;
}

vvp_fun_signal_object_aa::vvp_fun_signal_object_aa(अचिह्नित size)
: vvp_fun_signal_object(size)
{
      context_idx_ = vpip_सङ्कलय_item_गन्तव्यम्_context(एतत्, vpip_peek_context_scope());
}

vvp_fun_signal_object_aa::~vvp_fun_signal_object_aa()
{
      प्रतिज्ञा(0);
}

शून्य vvp_fun_signal_object_aa::alloc_instance(vvp_context_t context)
{
      vvp_object_t*द्वयङ्काः = नूतन vvp_object_t;
      vvp_समुच्चयः_context_item(context, context_idx_, द्वयङ्काः);
      द्वयङ्काः->reset();
}

शून्य vvp_fun_signal_object_aa::reset_instance(vvp_context_t context)
{
      vvp_object_t*द्वयङ्काः = static_cast<vvp_object_t*>
	    (vvp_get_context_item(context, context_idx_));
      द्वयङ्काः->reset();
}

vvp_object_t vvp_fun_signal_object_aa::get_object() ध्रुव
{
      ध्रुव vvp_object_t*द्वयङ्काः = static_cast<vvp_object_t*>
	    (vthread_get_rd_context_item(context_idx_));
      प्रत्यागम *द्वयङ्काः;
}

शून्य vvp_fun_signal_object_aa::recv_object(vvp_net_ptr_t ptr, vvp_object_t bit,
					   vvp_context_t context)
{
      प्रतिज्ञा(ptr.पत्तनम्() == 0);
      प्रतिज्ञा(context);

      vvp_object_t*द्वयङ्काः = static_cast<vvp_object_t*>
	    (vvp_get_context_item(context, context_idx_));

      यदि (*द्वयङ्काः != bit) {
	    *द्वयङ्काः = bit;
	    ptr.ptr()->send_object(bit, context);
      }
}

अचिह्नित vvp_fun_signal_object_aa::मानम्_आकार() ध्रुव
{
      प्रत्यागम 1;
}

vvp_bit4_t vvp_fun_signal_object_aa::value(अचिह्नित) ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम BIT4_X;
}

vvp_scalar_t vvp_fun_signal_object_aa::scalar_मानम्(अचिह्नित) ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम vvp_scalar_t();
}

शून्य vvp_fun_signal_object_aa::vec4_मानम्(vvp_vector4_t&) ध्रुव
{
      प्रतिज्ञा(0);
}

शून्य* vvp_fun_signal_object_aa::संक्रियकः नूतन(std::size_t size)
{
      प्रत्यागम vvp_net_fun_t::heap_.alloc(size);
}

शून्य vvp_fun_signal_object_aa::संक्रियकः विलोपय(शून्य*)
{
      प्रतिज्ञा(0);
}

  /* **** */

vvp_fun_force::vvp_fun_force()
{
}

vvp_fun_force::~vvp_fun_force()
{
}

शून्य vvp_fun_force::recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
			      vvp_context_t)
{
      प्रतिज्ञा(ptr.पत्तनम्() == 0);
      vvp_net_t*net = ptr.ptr();

      vvp_net_t*dst = net->पत्तनम्[3].ptr();
      प्रतिज्ञा(dst->fil);

      dst->force_vec4(coerce_गन्तव्यम्_विस्तारः(bit, dst->fil->filter_आकार()), vvp_vector2_t(vvp_vector2_t::FILL1, dst->fil->filter_आकार()));
}

शून्य vvp_fun_force::recv_real(vvp_net_ptr_t ptr, द्विगुण bit, vvp_context_t)
{
      प्रतिज्ञा(ptr.पत्तनम्() == 0);
      vvp_net_t*net = ptr.ptr();
      vvp_net_t*dst = net->पत्तनम्[3].ptr();
      dst->force_real(bit, vvp_vector2_t(vvp_vector2_t::FILL1, 1));
}

vvp_wire_आधारः::vvp_wire_आधारः()
{
}

vvp_wire_आधारः::~vvp_wire_आधारः()
{
}

vvp_bit4_t vvp_wire_आधारः::driven_मानम्(अचिह्नित) ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम BIT4_X;
}

तार्किक vvp_wire_आधारः::is_forced(अचिह्नित) ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम असत्य;
}

vvp_wire_vec4::vvp_wire_vec4(अचिह्नित wid, vvp_bit4_t init)
: bits4_(wid, init)
{
      needs_init_ = सत्य;
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec4(ध्रुव vvp_vector4_t&bit, vvp_vector4_t&rep,
						 अचिह्नित आधारः, अचिह्नित vwid)
{
	// Special case! the input bit is 0 wid. Interpret this as a
	// vector of BIT4_X to match the width of the bits4_ vector.
	// FIXME! This is a hack to work around some buggy gate
	// implementations! This should be removed!
      यदि (आधारः==0 && vwid==0) {
	    vvp_vector4_t tmp (bits4_.size(), BIT4_X);
	    यदि (bits4_ .eeq(tmp) && !needs_init_) प्रत्यागम STOP;
	    bits4_ = tmp;
	    needs_init_ = असत्य;
	    प्रत्यागम filter_आवरणम्(tmp, force4_, rep, 0);
      }

      यदि (vwid != bits4_.size()) {
	    cerr << "Internal error: Input vector expected width="
		 << bits4_.size() << ", got "
		 << "bit=" << bit << ", base=" << आधारः << ", vwid=" << vwid
		 << पङ्क्त्यन्तः;
      }
      प्रतिज्ञा(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      यदि (आधारः==0 && bit.size()==vwid) {
	    यदि (bits4_ .eeq( bit ) && !needs_init_) प्रत्यागम STOP;
	    bits4_ = bit;
      } अन्यथा {
	    तार्किक rc = bits4_.समुच्चयः_vec(आधारः, bit);
	    यदि (rc == असत्य && !needs_init_) प्रत्यागम STOP;
      }

      needs_init_ = असत्य;
      प्रत्यागम filter_आवरणम्(bit, force4_, rep, आधारः);
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec8(ध्रुव vvp_vector8_t&bit,
                                                 vvp_vector8_t&rep,
                                                 अचिह्नित आधारः,
                                                 अचिह्नित vwid)
{
      प्रतिज्ञा(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      vvp_vector4_t bit4 (reduce4(bit));
      यदि (आधारः==0 && bit4.size()==vwid) {
	    यदि (bits4_ .eeq( bit4 ) && !needs_init_) प्रत्यागम STOP;
	    bits4_ = bit4;
      } अन्यथा {
	    तार्किक rc = bits4_.समुच्चयः_vec(आधारः, bit4);
	    यदि (rc == असत्य && !needs_init_) प्रत्यागम STOP;
      }

      needs_init_ = असत्य;
      प्रत्यागम filter_आवरणम्(bit, vvp_vector8_t(force4_,6,6), rep, आधारः);
}

अचिह्नित vvp_wire_vec4::filter_आकार() ध्रुव
{
      प्रत्यागम bits4_.size();
}

शून्य vvp_wire_vec4::force_fil_vec4(ध्रुव vvp_vector4_t&val, ध्रुव vvp_vector2_t&आवरणम्)
{
      force_आवरणम्(आवरणम्);

      यदि (force4_.size() == 0) {
	    force4_ = val;
      } अन्यथा {
	    प्रति (अचिह्नित idx = 0; idx < आवरणम्.size() ; idx += 1) {
		  यदि (आवरणम्.value(idx) == 0)
			अनुवर्त;

		  force4_.समुच्चयः_bit(idx, val.value(idx));
	    }
      }
      सततम्_vpi_callbacks();
}

शून्य vvp_wire_vec4::force_fil_vec8(ध्रुव vvp_vector8_t&, ध्रुव vvp_vector2_t&)
{
      प्रतिज्ञा(0);
}

शून्य vvp_wire_vec4::force_fil_real(द्विगुण, ध्रुव vvp_vector2_t&)
{
      प्रतिज्ञा(0);
}

शून्य vvp_wire_vec4::release(vvp_net_ptr_t ptr, तार्किक net_flag)
{
      vvp_vector2_t आवरणम् (vvp_vector2_t::FILL1, bits4_.size());
      यदि (net_flag) {
	      // Wires revert to their unforced value after release.
            release_आवरणम्(आवरणम्);
	    needs_init_ = ! force4_ .eeq(bits4_);
	    ptr.ptr()->send_vec4(bits4_, 0);
	    सततम्_vpi_callbacks();
      } अन्यथा {
	      // Variables keep the current value.
	    vvp_vector4_t res (bits4_.size());
	    प्रति (अचिह्नित idx=0; idx<bits4_.size(); idx += 1)
		  res.समुच्चयः_bit(idx,value(idx));
            release_आवरणम्(आवरणम्);
	    ptr.ptr()->fun->recv_vec4(ptr, res, 0);
      }
}

शून्य vvp_wire_vec4::release_pv(vvp_net_ptr_t ptr, अचिह्नित आधारः, अचिह्नित wid, तार्किक net_flag)
{
      प्रतिज्ञा(bits4_.size() >= आधारः + wid);

      vvp_vector2_t आवरणम् (vvp_vector2_t::FILL0, bits4_.size());
      प्रति (अचिह्नित idx = 0 ; idx < wid ; idx += 1)
	    आवरणम्.समुच्चयः_bit(आधारः+idx, 1);

      यदि (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_आवरणम्(आवरणम्);
	    needs_init_ = ! force4_.subvalue(आधारः,wid) .eeq(bits4_.subvalue(आधारः,wid));
	    ptr.ptr()->send_vec4_pv(bits4_.subvalue(आधारः,wid),
				    आधारः, bits4_.size(), 0);
	    सततम्_vpi_callbacks();
      } अन्यथा {
	      // Variables keep the current value.
	    vvp_vector4_t res (wid);
	    प्रति (अचिह्नित idx=0; idx<wid; idx += 1)
		  res.समुच्चयः_bit(idx,value(आधारः+idx));
	    release_आवरणम्(आवरणम्);
	    ptr.ptr()->fun->recv_vec4_pv(ptr, res, आधारः, bits4_.size(), 0);
      }
}

अचिह्नित vvp_wire_vec4::मानम्_आकार() ध्रुव
{
      प्रत्यागम bits4_.size();
}

vvp_bit4_t vvp_wire_vec4::filtered_मानम्(अचिह्नित idx) ध्रुव
{
      यदि (test_force_आवरणम्(idx))
	    प्रत्यागम force4_.value(idx);
      अन्यथा
	    प्रत्यागम bits4_.value(idx);
}

vvp_bit4_t vvp_wire_vec4::value(अचिह्नित idx) ध्रुव
{
      प्रत्यागम filtered_मानम्(idx);
}

vvp_scalar_t vvp_wire_vec4::scalar_मानम्(अचिह्नित idx) ध्रुव
{
      प्रत्यागम vvp_scalar_t(value(idx),6,6);
}

शून्य vvp_wire_vec4::vec4_मानम्(vvp_vector4_t&val) ध्रुव
{
      val = bits4_;
      यदि (test_force_आवरणम्_is_zero())
	    प्रत्यागम;

      प्रति (अचिह्नित idx = 0 ; idx < bits4_.size() ; idx += 1)
	    val.समुच्चयः_bit(idx, filtered_मानम्(idx));
}

vvp_bit4_t vvp_wire_vec4::driven_मानम्(अचिह्नित idx) ध्रुव
{
      प्रत्यागम bits4_.value(idx);
}

तार्किक vvp_wire_vec4::is_forced(अचिह्नित idx) ध्रुव
{
      प्रत्यागम test_force_आवरणम्(idx);
}

vvp_wire_vec8::vvp_wire_vec8(अचिह्नित wid)
: bits8_(wid)
{
      needs_init_ = सत्य;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec4(ध्रुव vvp_vector4_t&bit,
                                                 vvp_vector4_t&rep,
                                                 अचिह्नित आधारः,
                                                 अचिह्नित vwid)
{
	// For now there is no support for a non-zero base.
      प्रतिज्ञा(0 == आधारः);
      प्रतिज्ञा(bits8_.size() == vwid);
      प्रतिज्ञा(bits8_.size() == bit.size());
	// QUESTION: Is it really correct to propagate a vec4 if this
	// is a vec8 node? In fact, it is really possible for a vec4
	// value to get through to a vec8 filter?
      vvp_vector8_t rep8;
      prop_t rc = filter_vec8(vvp_vector8_t(bit,6,6), rep8, 0, vwid);
      यदि (rc == REPL)
	    rep = reduce4(rep8);

      needs_init_ = असत्य;
      प्रत्यागम rc;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec8(ध्रुव vvp_vector8_t&bit, vvp_vector8_t&rep, अचिह्नित आधारः, अचिह्नित vwid)
{
      प्रतिज्ञा(vwid == bits8_.size());
	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      यदि (आधारः==0 && bit.size()==vwid) {
	    यदि (bits8_.eeq(bit) && !needs_init_) प्रत्यागम STOP;
	    bits8_ = bit;
      } अन्यथा {
	    यदि (bits8_.subvalue(आधारः, bit.size()).eeq(bit) &&
		!needs_init_) {
		  प्रत्यागम STOP;
	    }
	    यदि (bits8_.size() == 0)
		  bits8_ = vvp_vector8_t(vwid);
	    प्रतिज्ञा(bits8_.size() == vwid);
	    bits8_.समुच्चयः_vec(आधारः, bit);
      }
      needs_init_ = असत्य;
      प्रत्यागम filter_आवरणम्(bit, force8_, rep, आधारः);
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_निवेशः_vec8(ध्रुव vvp_vector8_t&bit, vvp_vector8_t&rep) ध्रुव
{
      प्रत्यागम filter_निवेशः_आवरणम्(bit, force8_, rep);
}

अचिह्नित vvp_wire_vec8::filter_आकार() ध्रुव
{
      प्रत्यागम bits8_.size();
}

शून्य vvp_wire_vec8::force_fil_vec4(ध्रुव vvp_vector4_t&val, ध्रुव vvp_vector2_t&आवरणम्)
{
      force_fil_vec8(vvp_vector8_t(val,6,6), आवरणम्);
}

शून्य vvp_wire_vec8::force_fil_vec8(ध्रुव vvp_vector8_t&val, ध्रुव vvp_vector2_t&आवरणम्)
{
      force_आवरणम्(आवरणम्);

      यदि (force8_.size() == 0) {
	    force8_ = val;
      } अन्यथा {
	    प्रति (अचिह्नित idx = 0; idx < आवरणम्.size() ; idx += 1) {
		  यदि (आवरणम्.value(idx) == 0)
			अनुवर्त;

		  force8_.समुच्चयः_bit(idx, val.value(idx));
	    }
      }
      सततम्_vpi_callbacks();
}

शून्य vvp_wire_vec8::force_fil_real(द्विगुण, ध्रुव vvp_vector2_t&)
{
      प्रतिज्ञा(0);
}

शून्य vvp_wire_vec8::release(vvp_net_ptr_t ptr, तार्किक net_flag)
{
	// Wires revert to their unforced value after release.
      vvp_vector2_t आवरणम् (vvp_vector2_t::FILL1, bits8_.size());
      release_आवरणम्(आवरणम्);
      यदि (net_flag) {
	    needs_init_ = !force8_ .eeq(bits8_);
	    ptr.ptr()->send_vec8(bits8_);
      } अन्यथा {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    प्रतिज्ञा(0);
//	    ptr.ptr()->fun->recv_vec8(ptr, force8_);
      }
}

शून्य vvp_wire_vec8::release_pv(vvp_net_ptr_t ptr, अचिह्नित आधारः, अचिह्नित wid, तार्किक net_flag)
{
      प्रतिज्ञा(bits8_.size() >= आधारः + wid);

      vvp_vector2_t आवरणम् (vvp_vector2_t::FILL0, bits8_.size());
      प्रति (अचिह्नित idx = 0 ; idx < wid ; idx += 1)
	    आवरणम्.समुच्चयः_bit(आधारः+idx, 1);

      release_आवरणम्(आवरणम्);

      यदि (net_flag) {
	    needs_init_ = !force8_.subvalue(आधारः,wid) .eeq((bits8_.subvalue(आधारः,wid)));
	    ptr.ptr()->send_vec8_pv(bits8_.subvalue(आधारः,wid),
				    आधारः, bits8_.size());
	    सततम्_vpi_callbacks();
      } अन्यथा {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    प्रतिज्ञा(0);
//	    ptr.ptr()->fun->recv_vec8_pv(ptr, force8_.subvalue(base,wid),
//					 base, force8_.size());
      }
}

अचिह्नित vvp_wire_vec8::मानम्_आकार() ध्रुव
{
      प्रत्यागम bits8_.size();
}

vvp_scalar_t vvp_wire_vec8::filtered_मानम्(अचिह्नित idx) ध्रुव
{
      यदि (test_force_आवरणम्(idx))
	    प्रत्यागम force8_.value(idx);
      अन्यथा
	    प्रत्यागम bits8_.value(idx);
}

vvp_bit4_t vvp_wire_vec8::value(अचिह्नित idx) ध्रुव
{
      प्रत्यागम filtered_मानम्(idx).value();
}

vvp_scalar_t vvp_wire_vec8::scalar_मानम्(अचिह्नित idx) ध्रुव
{
      प्रत्यागम filtered_मानम्(idx);
}

vvp_vector8_t vvp_wire_vec8::vec8_मानम्() ध्रुव
{
      vvp_vector8_t tmp = bits8_;
      प्रति (अचिह्नित idx = 0 ; idx < bits8_.size() ; idx += 1)
	    tmp.समुच्चयः_bit(idx, filtered_मानम्(idx));
      प्रत्यागम tmp;
}

शून्य vvp_wire_vec8::vec4_मानम्(vvp_vector4_t&val) ध्रुव
{
      val = reduce4(vec8_मानम्());
}

vvp_bit4_t vvp_wire_vec8::driven_मानम्(अचिह्नित idx) ध्रुव
{
      प्रत्यागम bits8_.value(idx).value();
}

तार्किक vvp_wire_vec8::is_forced(अचिह्नित idx) ध्रुव
{
      प्रत्यागम test_force_आवरणम्(idx);
}

vvp_wire_real::vvp_wire_real()
: bit_(0.0), force_(0.0)
{
}

vvp_net_fil_t::prop_t vvp_wire_real::filter_real(द्विगुण&bit)
{
      यदि (द्वयङ्काः_equal(bit_, bit) && !needs_init_) प्रत्यागम STOP;
      bit_ = bit;
      needs_init_ = असत्य;
      प्रत्यागम filter_आवरणम्(bit, force_);
}

अचिह्नित vvp_wire_real::filter_आकार() ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम 0;
}

शून्य vvp_wire_real::force_fil_vec4(ध्रुव vvp_vector4_t&, ध्रुव vvp_vector2_t&)
{
      प्रतिज्ञा(0);
}

शून्य vvp_wire_real::force_fil_vec8(ध्रुव vvp_vector8_t&, ध्रुव vvp_vector2_t&)
{
      प्रतिज्ञा(0);
}

शून्य vvp_wire_real::force_fil_real(द्विगुण val, ध्रुव vvp_vector2_t&आवरणम्)
{
      force_आवरणम्(आवरणम्);
      यदि (आवरणम्.value(0))
	    force_ = val;

      सततम्_vpi_callbacks();
}

शून्य vvp_wire_real::release(vvp_net_ptr_t ptr, तार्किक net_flag)
{
      vvp_vector2_t आवरणम् (vvp_vector2_t::FILL1, 1);
      यदि (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_आवरणम्(आवरणम्);
	    needs_init_ = !द्वयङ्काः_equal(force_, bit_);
	    ptr.ptr()->send_real(bit_, 0);
      } अन्यथा {
	      // Variables keep the current value.
	    द्विगुण res =  real_मानम्();
	    release_आवरणम्(आवरणम्);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
}

शून्य vvp_wire_real::release_pv(vvp_net_ptr_t, अचिह्नित, अचिह्नित, तार्किक)
{
      प्रतिज्ञा(0);
#यदि 0
	// A real is a single value. If for some reason this part release
	// can happen the following code should work correctly (requires
	// a base of 0 and a width of 1).
      vvp_vector2_t आवरणम् (vvp_vector2_t::FILL1, 1);
      प्रतिज्ञा(आधारः == 0 && wid == 1);

      यदि (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_आवरणम्(आवरणम्);
	    ptr.ptr()->send_real(bit_, 0);
      } अन्यथा {
	      // Variables keep the current value.
	    द्विगुण res =  real_मानम्();
	    release_आवरणम्(आवरणम्);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
#यदि_अन्तः
}

अचिह्नित vvp_wire_real::मानम्_आकार() ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम 1;
}

vvp_bit4_t vvp_wire_real::value(अचिह्नित) ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम BIT4_X;
}

vvp_scalar_t vvp_wire_real::scalar_मानम्(अचिह्नित) ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम vvp_scalar_t();
}

शून्य vvp_wire_real::vec4_मानम्(vvp_vector4_t&) ध्रुव
{
      प्रतिज्ञा(0);
}

द्विगुण vvp_wire_real::real_मानम्() ध्रुव
{
      यदि (test_force_आवरणम्(0))
	    प्रत्यागम force_;
      अन्यथा
	    प्रत्यागम bit_;
}

#यदि 0
vvp_wire_string::vvp_wire_string()
{
}

अचिह्नित vvp_wire_string::filter_आकार() ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम 0;
}

शून्य vvp_wire_string::force_fil_vec4(ध्रुव vvp_vector4_t&, ध्रुव vvp_vector2_t&)
{
      प्रतिज्ञा(0);
}
शून्य vvp_wire_string::force_fil_vec8(ध्रुव vvp_vector8_t&, ध्रुव vvp_vector2_t&)
{
      प्रतिज्ञा(0);
}
शून्य vvp_wire_string::force_fil_real(द्विगुण, ध्रुव vvp_vector2_t&)
{
      प्रतिज्ञा(0);
}

शून्य vvp_wire_string::release(vvp_net_ptr_t ptr, तार्किक net_flag)
{
      प्रतिज्ञा(0);
}

शून्य vvp_wire_string::release_pv(vvp_net_ptr_t, अचिह्नित, अचिह्नित, तार्किक)
{
      प्रतिज्ञा(0);
}

अचिह्नित vvp_wire_string::मानम्_आकार() ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम 1;
}

vvp_bit4_t vvp_wire_string::value(अचिह्नित) ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम BIT4_X;
}

vvp_scalar_t vvp_wire_string::scalar_मानम्(अचिह्नित) ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम vvp_scalar_t();
}

शून्य vvp_wire_string::vec4_मानम्(vvp_vector4_t&) ध्रुव
{
      प्रतिज्ञा(0);
}

द्विगुण vvp_wire_string::real_मानम्() ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम 0.0;
}
#यदि_अन्तः
