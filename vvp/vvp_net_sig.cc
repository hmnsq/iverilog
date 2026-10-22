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
#ifdef CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
# include  <map>
#endif

# include  <iostream>

using नामाकाश std;

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
template <वर्ग T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_mask_(ध्रुव T&val, ध्रुव T&force, T&filter, unsigned base)
{
      जइ (!test_force_mask_is_zero()) {
	      // Some bits are being forced. Go through the
	      // force_mask_ and force value to see which bits are
	      // propagated and which are kept from the forced
	      // value. Update the filter with the filtered result and
	      // return REPL to indicate that some bits have changed,
	      // or STOP if no bits change.
	    तार्किक propagate_flag = force_propagate_;
	    force_propagate_ = मिच्छ;
	    प्रतिज्ञा(force_mask_.size() == force.size());
	    प्रतिज्ञा((base+val.size()) <= force_mask_.size());

	    filter = val;
	    प्रति (unsigned idx = 0 ; idx < val.size() ; idx += 1) {
		  जइ (force_mask_.value(base+idx))
			filter.set_bit(idx, force.value(base+idx));
		  अण्णहा
			propagate_flag = सच्च;
	    }

	    जइ (propagate_flag) {
		  run_vpi_callbacks();
		  प्रत्यागम REPL;
	    } अण्णहा {
		  प्रत्यागम STOP;
	    }

      } अण्णहा {
	    run_vpi_callbacks();
	    प्रत्यागम PROP;
      }
}

template <वर्ग T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_mask_(T&val, T force)
{

      जइ (test_force_mask(0)) {
	    val = force;
	    run_vpi_callbacks();
	    प्रत्यागम REPL;
      }
      run_vpi_callbacks();
      प्रत्यागम PROP;
}

template <वर्ग T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_निवेशः_mask(ध्रुव T&val, ध्रुव T&force, T&rep) ध्रुव
{
      जइ (test_force_mask_is_zero())
	    प्रत्यागम PROP;

      प्रतिज्ञा(force_mask_.size() == force.size());

      rep = val;
      प्रति (unsigned idx = 0 ; idx < val.size() ; idx += 1) {
	    जइ (force_mask_.value(idx))
		  rep.set_bit(idx, force.value(idx));
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

शून्य vvp_net_t::force_vec4(ध्रुव vvp_vector4_t&val, ध्रुव vvp_vector2_t&mask)
{
      प्रतिज्ञा(fil);
      fil->force_fil_vec4(val, mask);
      fun->force_flag(मिच्छ);
      vvp_send_vec4(out_, val, 0);
}

शून्य vvp_net_t::force_vec8(ध्रुव vvp_vector8_t&val, ध्रुव vvp_vector2_t&mask)
{
      प्रतिज्ञा(fil);
      fil->force_fil_vec8(val, mask);
      fun->force_flag(मिच्छ);
      vvp_send_vec8(out_, val);
}

शून्य vvp_net_t::force_real(द्विगुण val, ध्रुव vvp_vector2_t&mask)
{
      प्रतिज्ञा(fil);
      fil->force_fil_real(val, mask);
      fun->force_flag(मिच्छ);
      vvp_send_real(out_, val, 0);
}

/* **** vvp_fun_signal methods **** */

vvp_fun_signal_base::vvp_fun_signal_base()
{
      continuous_assign_active_ = मिच्छ;
      needs_init_ = सच्च;
      cassign_link = 0;
      गणनम्_functors_sig += 1;
}

vvp_fun_signal4_sa::vvp_fun_signal4_sa(unsigned wid, vvp_bit4_t init)
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
      विकल्प (ptr.port()) {
	  पक्ष 0: // Normal input (feed from net, or set from process)
	      /* If we don't have a continuous assign mask then just
		 copy the bits, otherwise we need to see if there are
		 any holes in the mask so we can set those bits. */
	    जइ (assign_mask_.size() == 0) {
                  जइ (needs_init_ || !bits4_.eeq(bit)) {
			प्रतिज्ञा(bit.size() == bits4_.size());
			bits4_ = bit;
			needs_init_ = मिच्छ;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    } अण्णहा {
		  तार्किक changed = मिच्छ;
		  प्रतिज्ञा(bits4_.size() == assign_mask_.size());
		  प्रति (unsigned idx = 0 ;  idx < bit.size() ;  idx += 1) {
			जइ (idx >= bits4_.size()) भङ्ग;
			जइ (assign_mask_.value(idx)) अनुवर्त;
			bits4_.set_bit(idx, bit.value(idx));
			changed = सच्च;
		  }
		  जइ (changed) {
			needs_init_ = मिच्छ;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    }
	    भङ्ग;

	  पक्ष 1: // Continuous assign value
	      // Handle the simple case of the linked source being wider
	      // than this signal. Note we don't yet support the case of
	      // the linked source being narrower than this signal, or
	      // the case of an expression being assigned.
	    bits4_ = coerce_to_width(bit, bits4_.size());
	    assign_mask_ = vvp_vector2_t(vvp_vector2_t::FILL1, bits4_.size());
	    ptr.ptr()->send_vec4(bits4_, 0);
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    प्रतिज्ञा(0);
	    भङ्ग;
      }
}

शून्य vvp_fun_signal4_sa::recv_vec8(vvp_net_ptr_t ptr, ध्रुव vvp_vector8_t&bit)
{
      recv_vec4(ptr, reduce4(bit), 0);
}

शून्य vvp_fun_signal4_sa::recv_vec4_pv(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
				      unsigned base, unsigned vwid, vvp_context_t)
{
      प्रतिज्ञा(bits4_.size() == vwid);
      unsigned wid = bit.size();

      विकल्प (ptr.port()) {
	  पक्ष 0: // Normal input
	    जइ (assign_mask_.size() == 0) {
                  प्रति (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
			जइ (base+idx >= bits4_.size()) भङ्ग;
			bits4_.set_bit(base+idx, bit.value(idx));
		  }
		  needs_init_ = मिच्छ;
		  ptr.ptr()->send_vec4(bits4_,0);
	    } अण्णहा {
		  तार्किक changed = मिच्छ;
		  प्रतिज्ञा(bits4_.size() == assign_mask_.size());
		  प्रति (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
			जइ (base+idx >= bits4_.size()) भङ्ग;
			जइ (assign_mask_.value(base+idx)) अनुवर्त;
			bits4_.set_bit(base+idx, bit.value(idx));
			changed = सच्च;
		  }
		  जइ (changed) {
			needs_init_ = मिच्छ;
			ptr.ptr()->send_vec4(bits4_,0);
		  }
	    }
	    भङ्ग;

	  पक्ष 1: // Continuous assign value
	    जइ (assign_mask_.size() == 0)
		  assign_mask_ = vvp_vector2_t(vvp_vector2_t::FILL0, bits4_.size());
	    प्रति (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  जइ (base+idx >= bits4_.size())
			भङ्ग;
		  bits4_.set_bit(base+idx, bit.value(idx));
		  assign_mask_.set_bit(base+idx, 1);
	    }
	    ptr.ptr()->send_vec4(bits4_,0);
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    प्रतिज्ञा(0);
	    भङ्ग;
      }
}

शून्य vvp_fun_signal4_sa::recv_vec8_pv(vvp_net_ptr_t ptr, ध्रुव vvp_vector8_t&bit,
				      unsigned base, unsigned vwid)
{
      recv_vec4_pv(ptr, reduce4(bit), base, vwid, 0);
}

शून्य vvp_fun_signal_base::deassign()
{
      continuous_assign_active_ = मिच्छ;
      assign_mask_ = vvp_vector2_t();
}

शून्य vvp_fun_signal_base::deassign_pv(unsigned base, unsigned wid)
{
      प्रति (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    assign_mask_.set_bit(base+idx, 0);
      }

      जइ (assign_mask_.is_zero()) {
	    assign_mask_ = vvp_vector2_t();
      }
}

शून्य automatic_signal_base::release(vvp_net_ptr_t,तार्किक)
{
      प्रतिज्ञा(0);
}

शून्य automatic_signal_base::release_pv(vvp_net_ptr_t,unsigned,unsigned,तार्किक)
{
      प्रतिज्ञा(0);
}

unsigned automatic_signal_base::filter_size() ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम(0);
}
शून्य automatic_signal_base::force_fil_vec4(ध्रुव vvp_vector4_t&, ध्रुव vvp_vector2_t&)
{
      प्रतिज्ञा(0);
}
शून्य automatic_signal_base::force_fil_vec8(ध्रुव vvp_vector8_t&, ध्रुव vvp_vector2_t&)
{
      प्रतिज्ञा(0);
}
शून्य automatic_signal_base::force_fil_real(द्विगुण, ध्रुव vvp_vector2_t&)
{
      प्रतिज्ञा(0);
}
शून्य automatic_signal_base::get_मानम्(संरचना t_vpi_मानम्*)
{
      प्रतिज्ञा(0);
}

ध्रुव vvp_vector4_t& vvp_fun_signal4_sa::vec4_unfiltered_मानम्() ध्रुव
{
      प्रत्यागम bits4_;
}

vvp_fun_signal4_aa::vvp_fun_signal4_aa(unsigned wid, vvp_bit4_t init)
{
      context_idx_ = vpip_add_item_to_context(एतत्, vpip_peek_context_scope());
      size_ = wid;
      init_ = init;
}

vvp_fun_signal4_aa::~vvp_fun_signal4_aa()
{
      प्रतिज्ञा(0);
}

शून्य vvp_fun_signal4_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, नूतन vvp_vector4_t(size_, init_));
}

शून्य vvp_fun_signal4_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*bits = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      bits->fill_bits(init_);
}

#ifdef CHECK_WITH_VALGRIND
शून्य vvp_fun_signal4_aa::free_instance(vvp_context_t context)
{
      vvp_vector4_t*bits = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));
      delete bits;
}
#endif

/*
 * Continuous and forced assignments are not permitted on automatic
 * variables. So we only expect to receive on port 0.
 */
शून्य vvp_fun_signal4_aa::recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      प्रतिज्ञा(ptr.port() == 0);
      प्रतिज्ञा(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      जइ (!bits4->eeq(bit)) {
            *bits4 = bit;
            ptr.ptr()->send_vec4(*bits4, context);
      }
}

शून्य vvp_fun_signal4_aa::recv_vec4_pv(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
				      unsigned base, unsigned vwid, vvp_context_t context)
{
      प्रतिज्ञा(ptr.port() == 0);
      प्रतिज्ञा(size_ == vwid);
      प्रतिज्ञा(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      unsigned wid = bit.size();
      प्रति (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
            जइ (base+idx >= bits4->size()) भङ्ग;
            bits4->set_bit(base+idx, bit.value(idx));
      }
      ptr.ptr()->send_vec4(*bits4, context);
}

unsigned vvp_fun_signal4_aa::मानम्_size() ध्रुव
{
      प्रत्यागम size_;
}

vvp_bit4_t vvp_fun_signal4_aa::value(unsigned idx) ध्रुव
{
      ध्रुव vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      प्रत्यागम bits4->value(idx);
}

vvp_scalar_t vvp_fun_signal4_aa::scalar_मानम्(unsigned idx) ध्रुव
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

शून्य vvp_fun_signal4_aa::operator delete(शून्य*)
{
      प्रतिज्ञा(0);
}

/*
 * Testing for equality, we want a bitwise test instead of an
 * arithmetic test because we want to treat for example -0 different
 * from +0.
 */
तार्किक bits_equal(द्विगुण a, द्विगुण b)
{
      प्रत्यागम memcmp(&a, &b, परिमाण a) == 0;
}

vvp_fun_signal_real_sa::vvp_fun_signal_real_sa()
{
      bits_ = 0.0;
}

द्विगुण vvp_fun_signal_real_sa::real_unfiltered_मानम्() ध्रुव
{
      प्रत्यागम bits_;
}

शून्य vvp_fun_signal_real_sa::recv_real(vvp_net_ptr_t ptr, द्विगुण bit,
                                       vvp_context_t)
{
      विकल्प (ptr.port()) {
	  पक्ष 0:
	    जइ (!continuous_assign_active_) {
                  जइ (needs_init_ || !bits_equal(bits_, bit)) {
			bits_ = bit;
			needs_init_ = मिच्छ;
			ptr.ptr()->send_real(bit, 0);
		  }
	    }
	    भङ्ग;

	  पक्ष 1: // Continuous assign value
	    continuous_assign_active_ = सच्च;
	    bits_ = bit;
	    ptr.ptr()->send_real(bit, 0);
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    प्रतिज्ञा(0);
	    भङ्ग;
      }
}

vvp_fun_signal_real_aa::vvp_fun_signal_real_aa()
{
      context_idx_ = vpip_add_item_to_context(एतत्, vpip_peek_context_scope());
}

vvp_fun_signal_real_aa::~vvp_fun_signal_real_aa()
{
      प्रतिज्ञा(0);
}

शून्य vvp_fun_signal_real_aa::alloc_instance(vvp_context_t context)
{
      द्विगुण*bits = नूतन द्विगुण;
      vvp_set_context_item(context, context_idx_, bits);

      *bits = 0.0;
}

शून्य vvp_fun_signal_real_aa::reset_instance(vvp_context_t context)
{
      द्विगुण*bits = static_cast<द्विगुण*>
            (vvp_get_context_item(context, context_idx_));

      *bits = 0.0;
}

#ifdef CHECK_WITH_VALGRIND
शून्य vvp_fun_signal_real_aa::free_instance(vvp_context_t context)
{
      द्विगुण*bits = static_cast<द्विगुण*>
            (vvp_get_context_item(context, context_idx_));
      delete bits;
}
#endif

द्विगुण vvp_fun_signal_real_aa::real_unfiltered_मानम्() ध्रुव
{
      ध्रुव द्विगुण*bits = static_cast<द्विगुण*>
            (vthread_get_rd_context_item(context_idx_));

      प्रत्यागम *bits;
}

द्विगुण vvp_fun_signal_real_aa::real_मानम्() ध्रुव
{
      प्रत्यागम real_unfiltered_मानम्();
}

शून्य vvp_fun_signal_real_aa::recv_real(vvp_net_ptr_t ptr, द्विगुण bit,
                                       vvp_context_t context)
{
      प्रतिज्ञा(ptr.port() == 0);
      प्रतिज्ञा(context);

      द्विगुण*bits = static_cast<द्विगुण*>
            (vvp_get_context_item(context, context_idx_));

      जइ (!bits_equal(*bits,bit)) {
            *bits = bit;
            ptr.ptr()->send_real(bit, context);
      }
}

unsigned vvp_fun_signal_real_aa::मानम्_size() ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम 1;
}

vvp_bit4_t vvp_fun_signal_real_aa::value(unsigned) ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम BIT4_X;
}

vvp_scalar_t vvp_fun_signal_real_aa::scalar_मानम्(unsigned) ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम vvp_scalar_t();
}

शून्य vvp_fun_signal_real_aa::vec4_मानम्(vvp_vector4_t&) ध्रुव
{
      प्रतिज्ञा(0);
}

शून्य* vvp_fun_signal_real_aa::operator नूतन(std::size_t size)
{
      प्रत्यागम vvp_net_fun_t::heap_.alloc(size);
}

शून्य vvp_fun_signal_real_aa::operator delete(शून्य*)
{
      प्रतिज्ञा(0);
}


vvp_fun_signal_string_sa::vvp_fun_signal_string_sa()
{
}

शून्य vvp_fun_signal_string_sa::recv_string(vvp_net_ptr_t ptr, ध्रुव std::string&bit,
					   vvp_context_t)
{
      प्रतिज्ञा(ptr.port() == 0);

      जइ (needs_init_ || मानम्_2 != bit) {
	    मानम्_2 = bit;
	    needs_init_ = मिच्छ;

	    ptr.ptr()->send_string(bit, 0);
      }
}

ध्रुव string& vvp_fun_signal_string_sa::get_string() ध्रुव
{
      प्रत्यागम मानम्_2;
}

vvp_fun_signal_string_aa::vvp_fun_signal_string_aa()
{
      context_idx_ = vpip_add_item_to_context(एतत्, vpip_peek_context_scope());
}

vvp_fun_signal_string_aa::~vvp_fun_signal_string_aa()
{
      प्रतिज्ञा(0);
}

शून्य vvp_fun_signal_string_aa::alloc_instance(vvp_context_t context)
{
      string*bits = नूतन std::string;
      vvp_set_context_item(context, context_idx_, bits);
      *bits = "";
}

शून्य vvp_fun_signal_string_aa::reset_instance(vvp_context_t context)
{
      string*bits = static_cast<std::string*>
	    (vvp_get_context_item(context, context_idx_));
      *bits = "";
}

#ifdef CHECK_WITH_VALGRIND
शून्य vvp_fun_signal_string_aa::free_instance(vvp_context_t context)
{
      string*bits = static_cast<std::string*>
            (vvp_get_context_item(context, context_idx_));
      delete bits;
}
#endif

शून्य vvp_fun_signal_string_aa::recv_string(vvp_net_ptr_t ptr, ध्रुव std::string&bit, vvp_context_t context)
{
      प्रतिज्ञा(ptr.port() == 0);
      प्रतिज्ञा(context);

      string*bits = static_cast<std::string*>
	    (vvp_get_context_item(context, context_idx_));

      जइ (*bits != bit) {
	    *bits = bit;
	    ptr.ptr()->send_string(bit, context);
      }
}

unsigned vvp_fun_signal_string_aa::मानम्_size() ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम 1;
}

vvp_bit4_t vvp_fun_signal_string_aa::value(unsigned) ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम BIT4_X;
}

vvp_scalar_t vvp_fun_signal_string_aa::scalar_मानम्(unsigned) ध्रुव
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
      ध्रुव string*bits = static_cast<std::string*>
            (vthread_get_rd_context_item(context_idx_));

      प्रत्यागम *bits;
}

शून्य* vvp_fun_signal_string_aa::operator नूतन(std::size_t size)
{
      प्रत्यागम vvp_net_fun_t::heap_.alloc(size);
}

शून्य vvp_fun_signal_string_aa::operator delete(शून्य*)
{
      प्रतिज्ञा(0);
}

  /* OBJECT signals */

vvp_fun_signal_object_sa::vvp_fun_signal_object_sa(unsigned size)
: vvp_fun_signal_object(size)
{
}

#ifdef CHECK_WITH_VALGRIND
शून्य vvp_fun_signal_object_aa::free_instance(vvp_context_t context)
{
      vvp_object_t*bits = static_cast<vvp_object_t*>
            (vvp_get_context_item(context, context_idx_));
      delete bits;
}
#endif

शून्य vvp_fun_signal_object_sa::recv_object(vvp_net_ptr_t ptr, vvp_object_t bit,
					   vvp_context_t)
{
      प्रतिज्ञा(ptr.port() == 0);

      जइ (needs_init_ || मानम्_2 != bit) {
	    मानम्_2 = bit;
	    needs_init_ = मिच्छ;

	    ptr.ptr()->send_object(bit, 0);
      }
}

vvp_object_t vvp_fun_signal_object_sa::get_object() ध्रुव
{
      प्रत्यागम मानम्_2;
}

vvp_fun_signal_object_aa::vvp_fun_signal_object_aa(unsigned size)
: vvp_fun_signal_object(size)
{
      context_idx_ = vpip_add_item_to_context(एतत्, vpip_peek_context_scope());
}

vvp_fun_signal_object_aa::~vvp_fun_signal_object_aa()
{
      प्रतिज्ञा(0);
}

शून्य vvp_fun_signal_object_aa::alloc_instance(vvp_context_t context)
{
      vvp_object_t*bits = नूतन vvp_object_t;
      vvp_set_context_item(context, context_idx_, bits);
      bits->reset();
}

शून्य vvp_fun_signal_object_aa::reset_instance(vvp_context_t context)
{
      vvp_object_t*bits = static_cast<vvp_object_t*>
	    (vvp_get_context_item(context, context_idx_));
      bits->reset();
}

vvp_object_t vvp_fun_signal_object_aa::get_object() ध्रुव
{
      ध्रुव vvp_object_t*bits = static_cast<vvp_object_t*>
	    (vthread_get_rd_context_item(context_idx_));
      प्रत्यागम *bits;
}

शून्य vvp_fun_signal_object_aa::recv_object(vvp_net_ptr_t ptr, vvp_object_t bit,
					   vvp_context_t context)
{
      प्रतिज्ञा(ptr.port() == 0);
      प्रतिज्ञा(context);

      vvp_object_t*bits = static_cast<vvp_object_t*>
	    (vvp_get_context_item(context, context_idx_));

      जइ (*bits != bit) {
	    *bits = bit;
	    ptr.ptr()->send_object(bit, context);
      }
}

unsigned vvp_fun_signal_object_aa::मानम्_size() ध्रुव
{
      प्रत्यागम 1;
}

vvp_bit4_t vvp_fun_signal_object_aa::value(unsigned) ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम BIT4_X;
}

vvp_scalar_t vvp_fun_signal_object_aa::scalar_मानम्(unsigned) ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम vvp_scalar_t();
}

शून्य vvp_fun_signal_object_aa::vec4_मानम्(vvp_vector4_t&) ध्रुव
{
      प्रतिज्ञा(0);
}

शून्य* vvp_fun_signal_object_aa::operator नूतन(std::size_t size)
{
      प्रत्यागम vvp_net_fun_t::heap_.alloc(size);
}

शून्य vvp_fun_signal_object_aa::operator delete(शून्य*)
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
      प्रतिज्ञा(ptr.port() == 0);
      vvp_net_t*net = ptr.ptr();

      vvp_net_t*dst = net->port[3].ptr();
      प्रतिज्ञा(dst->fil);

      dst->force_vec4(coerce_to_width(bit, dst->fil->filter_size()), vvp_vector2_t(vvp_vector2_t::FILL1, dst->fil->filter_size()));
}

शून्य vvp_fun_force::recv_real(vvp_net_ptr_t ptr, द्विगुण bit, vvp_context_t)
{
      प्रतिज्ञा(ptr.port() == 0);
      vvp_net_t*net = ptr.ptr();
      vvp_net_t*dst = net->port[3].ptr();
      dst->force_real(bit, vvp_vector2_t(vvp_vector2_t::FILL1, 1));
}

vvp_wire_base::vvp_wire_base()
{
}

vvp_wire_base::~vvp_wire_base()
{
}

vvp_bit4_t vvp_wire_base::driven_मानम्(unsigned) ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम BIT4_X;
}

तार्किक vvp_wire_base::is_forced(unsigned) ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम मिच्छ;
}

vvp_wire_vec4::vvp_wire_vec4(unsigned wid, vvp_bit4_t init)
: bits4_(wid, init)
{
      needs_init_ = सच्च;
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec4(ध्रुव vvp_vector4_t&bit, vvp_vector4_t&rep,
						 unsigned base, unsigned vwid)
{
	// Special case! the input bit is 0 wid. Interpret this as a
	// vector of BIT4_X to match the width of the bits4_ vector.
	// FIXME! This is a hack to work around some buggy gate
	// implementations! This should be removed!
      जइ (base==0 && vwid==0) {
	    vvp_vector4_t tmp (bits4_.size(), BIT4_X);
	    जइ (bits4_ .eeq(tmp) && !needs_init_) प्रत्यागम STOP;
	    bits4_ = tmp;
	    needs_init_ = मिच्छ;
	    प्रत्यागम filter_mask_(tmp, force4_, rep, 0);
      }

      जइ (vwid != bits4_.size()) {
	    cerr << "Internal error: Input vector expected width="
		 << bits4_.size() << ", got "
		 << "bit=" << bit << ", base=" << base << ", vwid=" << vwid
		 << endl;
      }
      प्रतिज्ञा(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      जइ (base==0 && bit.size()==vwid) {
	    जइ (bits4_ .eeq( bit ) && !needs_init_) प्रत्यागम STOP;
	    bits4_ = bit;
      } अण्णहा {
	    तार्किक rc = bits4_.set_vec(base, bit);
	    जइ (rc == मिच्छ && !needs_init_) प्रत्यागम STOP;
      }

      needs_init_ = मिच्छ;
      प्रत्यागम filter_mask_(bit, force4_, rep, base);
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec8(ध्रुव vvp_vector8_t&bit,
                                                 vvp_vector8_t&rep,
                                                 unsigned base,
                                                 unsigned vwid)
{
      प्रतिज्ञा(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      vvp_vector4_t bit4 (reduce4(bit));
      जइ (base==0 && bit4.size()==vwid) {
	    जइ (bits4_ .eeq( bit4 ) && !needs_init_) प्रत्यागम STOP;
	    bits4_ = bit4;
      } अण्णहा {
	    तार्किक rc = bits4_.set_vec(base, bit4);
	    जइ (rc == मिच्छ && !needs_init_) प्रत्यागम STOP;
      }

      needs_init_ = मिच्छ;
      प्रत्यागम filter_mask_(bit, vvp_vector8_t(force4_,6,6), rep, base);
}

unsigned vvp_wire_vec4::filter_size() ध्रुव
{
      प्रत्यागम bits4_.size();
}

शून्य vvp_wire_vec4::force_fil_vec4(ध्रुव vvp_vector4_t&val, ध्रुव vvp_vector2_t&mask)
{
      force_mask(mask);

      जइ (force4_.size() == 0) {
	    force4_ = val;
      } अण्णहा {
	    प्रति (unsigned idx = 0; idx < mask.size() ; idx += 1) {
		  जइ (mask.value(idx) == 0)
			अनुवर्त;

		  force4_.set_bit(idx, val.value(idx));
	    }
      }
      run_vpi_callbacks();
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
      vvp_vector2_t mask (vvp_vector2_t::FILL1, bits4_.size());
      जइ (net_flag) {
	      // Wires revert to their unforced value after release.
            release_mask(mask);
	    needs_init_ = ! force4_ .eeq(bits4_);
	    ptr.ptr()->send_vec4(bits4_, 0);
	    run_vpi_callbacks();
      } अण्णहा {
	      // Variables keep the current value.
	    vvp_vector4_t res (bits4_.size());
	    प्रति (unsigned idx=0; idx<bits4_.size(); idx += 1)
		  res.set_bit(idx,value(idx));
            release_mask(mask);
	    ptr.ptr()->fun->recv_vec4(ptr, res, 0);
      }
}

शून्य vvp_wire_vec4::release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, तार्किक net_flag)
{
      प्रतिज्ञा(bits4_.size() >= base + wid);

      vvp_vector2_t mask (vvp_vector2_t::FILL0, bits4_.size());
      प्रति (unsigned idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      जइ (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_mask(mask);
	    needs_init_ = ! force4_.subvalue(base,wid) .eeq(bits4_.subvalue(base,wid));
	    ptr.ptr()->send_vec4_pv(bits4_.subvalue(base,wid),
				    base, bits4_.size(), 0);
	    run_vpi_callbacks();
      } अण्णहा {
	      // Variables keep the current value.
	    vvp_vector4_t res (wid);
	    प्रति (unsigned idx=0; idx<wid; idx += 1)
		  res.set_bit(idx,value(base+idx));
	    release_mask(mask);
	    ptr.ptr()->fun->recv_vec4_pv(ptr, res, base, bits4_.size(), 0);
      }
}

unsigned vvp_wire_vec4::मानम्_size() ध्रुव
{
      प्रत्यागम bits4_.size();
}

vvp_bit4_t vvp_wire_vec4::filtered_मानम्(unsigned idx) ध्रुव
{
      जइ (test_force_mask(idx))
	    प्रत्यागम force4_.value(idx);
      अण्णहा
	    प्रत्यागम bits4_.value(idx);
}

vvp_bit4_t vvp_wire_vec4::value(unsigned idx) ध्रुव
{
      प्रत्यागम filtered_मानम्(idx);
}

vvp_scalar_t vvp_wire_vec4::scalar_मानम्(unsigned idx) ध्रुव
{
      प्रत्यागम vvp_scalar_t(value(idx),6,6);
}

शून्य vvp_wire_vec4::vec4_मानम्(vvp_vector4_t&val) ध्रुव
{
      val = bits4_;
      जइ (test_force_mask_is_zero())
	    प्रत्यागम;

      प्रति (unsigned idx = 0 ; idx < bits4_.size() ; idx += 1)
	    val.set_bit(idx, filtered_मानम्(idx));
}

vvp_bit4_t vvp_wire_vec4::driven_मानम्(unsigned idx) ध्रुव
{
      प्रत्यागम bits4_.value(idx);
}

तार्किक vvp_wire_vec4::is_forced(unsigned idx) ध्रुव
{
      प्रत्यागम test_force_mask(idx);
}

vvp_wire_vec8::vvp_wire_vec8(unsigned wid)
: bits8_(wid)
{
      needs_init_ = सच्च;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec4(ध्रुव vvp_vector4_t&bit,
                                                 vvp_vector4_t&rep,
                                                 unsigned base,
                                                 unsigned vwid)
{
	// For now there is no support for a non-zero base.
      प्रतिज्ञा(0 == base);
      प्रतिज्ञा(bits8_.size() == vwid);
      प्रतिज्ञा(bits8_.size() == bit.size());
	// QUESTION: Is it really correct to propagate a vec4 if this
	// is a vec8 node? In fact, it is really possible for a vec4
	// value to get through to a vec8 filter?
      vvp_vector8_t rep8;
      prop_t rc = filter_vec8(vvp_vector8_t(bit,6,6), rep8, 0, vwid);
      जइ (rc == REPL)
	    rep = reduce4(rep8);

      needs_init_ = मिच्छ;
      प्रत्यागम rc;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec8(ध्रुव vvp_vector8_t&bit, vvp_vector8_t&rep, unsigned base, unsigned vwid)
{
      प्रतिज्ञा(vwid == bits8_.size());
	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      जइ (base==0 && bit.size()==vwid) {
	    जइ (bits8_.eeq(bit) && !needs_init_) प्रत्यागम STOP;
	    bits8_ = bit;
      } अण्णहा {
	    जइ (bits8_.subvalue(base, bit.size()).eeq(bit) &&
		!needs_init_) {
		  प्रत्यागम STOP;
	    }
	    जइ (bits8_.size() == 0)
		  bits8_ = vvp_vector8_t(vwid);
	    प्रतिज्ञा(bits8_.size() == vwid);
	    bits8_.set_vec(base, bit);
      }
      needs_init_ = मिच्छ;
      प्रत्यागम filter_mask_(bit, force8_, rep, base);
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_निवेशः_vec8(ध्रुव vvp_vector8_t&bit, vvp_vector8_t&rep) ध्रुव
{
      प्रत्यागम filter_निवेशः_mask(bit, force8_, rep);
}

unsigned vvp_wire_vec8::filter_size() ध्रुव
{
      प्रत्यागम bits8_.size();
}

शून्य vvp_wire_vec8::force_fil_vec4(ध्रुव vvp_vector4_t&val, ध्रुव vvp_vector2_t&mask)
{
      force_fil_vec8(vvp_vector8_t(val,6,6), mask);
}

शून्य vvp_wire_vec8::force_fil_vec8(ध्रुव vvp_vector8_t&val, ध्रुव vvp_vector2_t&mask)
{
      force_mask(mask);

      जइ (force8_.size() == 0) {
	    force8_ = val;
      } अण्णहा {
	    प्रति (unsigned idx = 0; idx < mask.size() ; idx += 1) {
		  जइ (mask.value(idx) == 0)
			अनुवर्त;

		  force8_.set_bit(idx, val.value(idx));
	    }
      }
      run_vpi_callbacks();
}

शून्य vvp_wire_vec8::force_fil_real(द्विगुण, ध्रुव vvp_vector2_t&)
{
      प्रतिज्ञा(0);
}

शून्य vvp_wire_vec8::release(vvp_net_ptr_t ptr, तार्किक net_flag)
{
	// Wires revert to their unforced value after release.
      vvp_vector2_t mask (vvp_vector2_t::FILL1, bits8_.size());
      release_mask(mask);
      जइ (net_flag) {
	    needs_init_ = !force8_ .eeq(bits8_);
	    ptr.ptr()->send_vec8(bits8_);
      } अण्णहा {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    प्रतिज्ञा(0);
//	    ptr.ptr()->fun->recv_vec8(ptr, force8_);
      }
}

शून्य vvp_wire_vec8::release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, तार्किक net_flag)
{
      प्रतिज्ञा(bits8_.size() >= base + wid);

      vvp_vector2_t mask (vvp_vector2_t::FILL0, bits8_.size());
      प्रति (unsigned idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      release_mask(mask);

      जइ (net_flag) {
	    needs_init_ = !force8_.subvalue(base,wid) .eeq((bits8_.subvalue(base,wid)));
	    ptr.ptr()->send_vec8_pv(bits8_.subvalue(base,wid),
				    base, bits8_.size());
	    run_vpi_callbacks();
      } अण्णहा {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    प्रतिज्ञा(0);
//	    ptr.ptr()->fun->recv_vec8_pv(ptr, force8_.subvalue(base,wid),
//					 base, force8_.size());
      }
}

unsigned vvp_wire_vec8::मानम्_size() ध्रुव
{
      प्रत्यागम bits8_.size();
}

vvp_scalar_t vvp_wire_vec8::filtered_मानम्(unsigned idx) ध्रुव
{
      जइ (test_force_mask(idx))
	    प्रत्यागम force8_.value(idx);
      अण्णहा
	    प्रत्यागम bits8_.value(idx);
}

vvp_bit4_t vvp_wire_vec8::value(unsigned idx) ध्रुव
{
      प्रत्यागम filtered_मानम्(idx).value();
}

vvp_scalar_t vvp_wire_vec8::scalar_मानम्(unsigned idx) ध्रुव
{
      प्रत्यागम filtered_मानम्(idx);
}

vvp_vector8_t vvp_wire_vec8::vec8_मानम्() ध्रुव
{
      vvp_vector8_t tmp = bits8_;
      प्रति (unsigned idx = 0 ; idx < bits8_.size() ; idx += 1)
	    tmp.set_bit(idx, filtered_मानम्(idx));
      प्रत्यागम tmp;
}

शून्य vvp_wire_vec8::vec4_मानम्(vvp_vector4_t&val) ध्रुव
{
      val = reduce4(vec8_मानम्());
}

vvp_bit4_t vvp_wire_vec8::driven_मानम्(unsigned idx) ध्रुव
{
      प्रत्यागम bits8_.value(idx).value();
}

तार्किक vvp_wire_vec8::is_forced(unsigned idx) ध्रुव
{
      प्रत्यागम test_force_mask(idx);
}

vvp_wire_real::vvp_wire_real()
: bit_(0.0), force_(0.0)
{
}

vvp_net_fil_t::prop_t vvp_wire_real::filter_real(द्विगुण&bit)
{
      जइ (bits_equal(bit_, bit) && !needs_init_) प्रत्यागम STOP;
      bit_ = bit;
      needs_init_ = मिच्छ;
      प्रत्यागम filter_mask_(bit, force_);
}

unsigned vvp_wire_real::filter_size() ध्रुव
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

शून्य vvp_wire_real::force_fil_real(द्विगुण val, ध्रुव vvp_vector2_t&mask)
{
      force_mask(mask);
      जइ (mask.value(0))
	    force_ = val;

      run_vpi_callbacks();
}

शून्य vvp_wire_real::release(vvp_net_ptr_t ptr, तार्किक net_flag)
{
      vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
      जइ (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_mask(mask);
	    needs_init_ = !bits_equal(force_, bit_);
	    ptr.ptr()->send_real(bit_, 0);
      } अण्णहा {
	      // Variables keep the current value.
	    द्विगुण res =  real_मानम्();
	    release_mask(mask);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
}

शून्य vvp_wire_real::release_pv(vvp_net_ptr_t, unsigned, unsigned, तार्किक)
{
      प्रतिज्ञा(0);
#जइ 0
	// A real is a single value. If for some reason this part release
	// can happen the following code should work correctly (requires
	// a base of 0 and a width of 1).
      vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
      प्रतिज्ञा(base == 0 && wid == 1);

      जइ (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_mask(mask);
	    ptr.ptr()->send_real(bit_, 0);
      } अण्णहा {
	      // Variables keep the current value.
	    द्विगुण res =  real_मानम्();
	    release_mask(mask);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
#endif
}

unsigned vvp_wire_real::मानम्_size() ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम 1;
}

vvp_bit4_t vvp_wire_real::value(unsigned) ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम BIT4_X;
}

vvp_scalar_t vvp_wire_real::scalar_मानम्(unsigned) ध्रुव
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
      जइ (test_force_mask(0))
	    प्रत्यागम force_;
      अण्णहा
	    प्रत्यागम bit_;
}

#जइ 0
vvp_wire_string::vvp_wire_string()
{
}

unsigned vvp_wire_string::filter_size() ध्रुव
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

शून्य vvp_wire_string::release_pv(vvp_net_ptr_t, unsigned, unsigned, तार्किक)
{
      प्रतिज्ञा(0);
}

unsigned vvp_wire_string::मानम्_size() ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम 1;
}

vvp_bit4_t vvp_wire_string::value(unsigned) ध्रुव
{
      प्रतिज्ञा(0);
      प्रत्यागम BIT4_X;
}

vvp_scalar_t vvp_wire_string::scalar_मानम्(unsigned) ध्रुव
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
#endif
