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
ढाँचा <वर्ग T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_mask_(अचर T&val, अचर T&force, T&filter, अचिन्हित base)
{
      यदि (!test_force_mask_is_zero()) {
	      // Some bits are being forced. Go through the
	      // force_mask_ and force value to see which bits are
	      // propagated and which are kept from the forced
	      // value. Update the filter with the filtered result and
	      // return REPL to indicate that some bits have changed,
	      // or STOP if no bits change.
	    तार्किक propagate_flag = force_propagate_;
	    force_propagate_ = असत्य;
	    प्रतिज्ञा(force_mask_.size() == force.size());
	    प्रतिज्ञा((base+val.size()) <= force_mask_.size());

	    filter = val;
	    लागि (अचिन्हित idx = 0 ; idx < val.size() ; idx += 1) {
		  यदि (force_mask_.value(base+idx))
			filter.set_bit(idx, force.value(base+idx));
		  अन्यथा
			propagate_flag = सत्य;
	    }

	    यदि (propagate_flag) {
		  run_vpi_callbacks();
		  फर्काऊ REPL;
	    } अन्यथा {
		  फर्काऊ STOP;
	    }

      } अन्यथा {
	    run_vpi_callbacks();
	    फर्काऊ PROP;
      }
}

ढाँचा <वर्ग T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_mask_(T&val, T force)
{

      यदि (test_force_mask(0)) {
	    val = force;
	    run_vpi_callbacks();
	    फर्काऊ REPL;
      }
      run_vpi_callbacks();
      फर्काऊ PROP;
}

ढाँचा <वर्ग T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_आगत_mask(अचर T&val, अचर T&force, T&rep) अचर
{
      यदि (test_force_mask_is_zero())
	    फर्काऊ PROP;

      प्रतिज्ञा(force_mask_.size() == force.size());

      rep = val;
      लागि (अचिन्हित idx = 0 ; idx < val.size() ; idx += 1) {
	    यदि (force_mask_.value(idx))
		  rep.set_bit(idx, force.value(idx));
      }

      फर्काऊ REPL;
}

vvp_signal_मान::~vvp_signal_मान()
{
}

द्विगुण vvp_signal_मान::real_मान() अचर
{
      प्रतिज्ञा(0);
      फर्काऊ 0;
}

शून्य vvp_net_t::force_vec4(अचर vvp_vector4_t&val, अचर vvp_vector2_t&mask)
{
      प्रतिज्ञा(fil);
      fil->force_fil_vec4(val, mask);
      fun->force_flag(असत्य);
      vvp_send_vec4(out_, val, 0);
}

शून्य vvp_net_t::force_vec8(अचर vvp_vector8_t&val, अचर vvp_vector2_t&mask)
{
      प्रतिज्ञा(fil);
      fil->force_fil_vec8(val, mask);
      fun->force_flag(असत्य);
      vvp_send_vec8(out_, val);
}

शून्य vvp_net_t::force_real(द्विगुण val, अचर vvp_vector2_t&mask)
{
      प्रतिज्ञा(fil);
      fil->force_fil_real(val, mask);
      fun->force_flag(असत्य);
      vvp_send_real(out_, val, 0);
}

/* **** vvp_fun_signal methods **** */

vvp_fun_signal_base::vvp_fun_signal_base()
{
      continuous_assign_active_ = असत्य;
      needs_init_ = सत्य;
      cassign_link = 0;
      गन्ती_functors_sig += 1;
}

vvp_fun_signal4_sa::vvp_fun_signal4_sa(अचिन्हित wid, vvp_bit4_t init)
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
शून्य vvp_fun_signal4_sa::recv_vec4(vvp_net_ptr_t ptr, अचर vvp_vector4_t&bit,
                                   vvp_context_t)
{
      विकल्प (ptr.port()) {
	  पक्ष 0: // Normal input (feed from net, or set from process)
	      /* If we don't have a continuous assign mask then just
		 copy the bits, otherwise we need to see if there are
		 any holes in the mask so we can set those bits. */
	    यदि (assign_mask_.size() == 0) {
                  यदि (needs_init_ || !bits4_.eeq(bit)) {
			प्रतिज्ञा(bit.size() == bits4_.size());
			bits4_ = bit;
			needs_init_ = असत्य;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    } अन्यथा {
		  तार्किक changed = असत्य;
		  प्रतिज्ञा(bits4_.size() == assign_mask_.size());
		  लागि (अचिन्हित idx = 0 ;  idx < bit.size() ;  idx += 1) {
			यदि (idx >= bits4_.size()) रोक;
			यदि (assign_mask_.value(idx)) जारी;
			bits4_.set_bit(idx, bit.value(idx));
			changed = सत्य;
		  }
		  यदि (changed) {
			needs_init_ = असत्य;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    }
	    रोक;

	  पक्ष 1: // Continuous assign value
	      // Handle the simple case of the linked source being wider
	      // than this signal. Note we don't yet support the case of
	      // the linked source being narrower than this signal, or
	      // the case of an expression being assigned.
	    bits4_ = coerce_to_width(bit, bits4_.size());
	    assign_mask_ = vvp_vector2_t(vvp_vector2_t::FILL1, bits4_.size());
	    ptr.ptr()->send_vec4(bits4_, 0);
	    रोक;

	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "Unsupported port type %u.\n", ptr.port());
	    प्रतिज्ञा(0);
	    रोक;
      }
}

शून्य vvp_fun_signal4_sa::recv_vec8(vvp_net_ptr_t ptr, अचर vvp_vector8_t&bit)
{
      recv_vec4(ptr, reduce4(bit), 0);
}

शून्य vvp_fun_signal4_sa::recv_vec4_pv(vvp_net_ptr_t ptr, अचर vvp_vector4_t&bit,
				      अचिन्हित base, अचिन्हित vwid, vvp_context_t)
{
      प्रतिज्ञा(bits4_.size() == vwid);
      अचिन्हित wid = bit.size();

      विकल्प (ptr.port()) {
	  पक्ष 0: // Normal input
	    यदि (assign_mask_.size() == 0) {
                  लागि (अचिन्हित idx = 0 ;  idx < wid ;  idx += 1) {
			यदि (base+idx >= bits4_.size()) रोक;
			bits4_.set_bit(base+idx, bit.value(idx));
		  }
		  needs_init_ = असत्य;
		  ptr.ptr()->send_vec4(bits4_,0);
	    } अन्यथा {
		  तार्किक changed = असत्य;
		  प्रतिज्ञा(bits4_.size() == assign_mask_.size());
		  लागि (अचिन्हित idx = 0 ;  idx < wid ;  idx += 1) {
			यदि (base+idx >= bits4_.size()) रोक;
			यदि (assign_mask_.value(base+idx)) जारी;
			bits4_.set_bit(base+idx, bit.value(idx));
			changed = सत्य;
		  }
		  यदि (changed) {
			needs_init_ = असत्य;
			ptr.ptr()->send_vec4(bits4_,0);
		  }
	    }
	    रोक;

	  पक्ष 1: // Continuous assign value
	    यदि (assign_mask_.size() == 0)
		  assign_mask_ = vvp_vector2_t(vvp_vector2_t::FILL0, bits4_.size());
	    लागि (अचिन्हित idx = 0 ;  idx < wid ;  idx += 1) {
		  यदि (base+idx >= bits4_.size())
			रोक;
		  bits4_.set_bit(base+idx, bit.value(idx));
		  assign_mask_.set_bit(base+idx, 1);
	    }
	    ptr.ptr()->send_vec4(bits4_,0);
	    रोक;

	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "Unsupported port type %u.\n", ptr.port());
	    प्रतिज्ञा(0);
	    रोक;
      }
}

शून्य vvp_fun_signal4_sa::recv_vec8_pv(vvp_net_ptr_t ptr, अचर vvp_vector8_t&bit,
				      अचिन्हित base, अचिन्हित vwid)
{
      recv_vec4_pv(ptr, reduce4(bit), base, vwid, 0);
}

शून्य vvp_fun_signal_base::deassign()
{
      continuous_assign_active_ = असत्य;
      assign_mask_ = vvp_vector2_t();
}

शून्य vvp_fun_signal_base::deassign_pv(अचिन्हित base, अचिन्हित wid)
{
      लागि (अचिन्हित idx = 0 ;  idx < wid ;  idx += 1) {
	    assign_mask_.set_bit(base+idx, 0);
      }

      यदि (assign_mask_.is_zero()) {
	    assign_mask_ = vvp_vector2_t();
      }
}

शून्य automatic_signal_base::release(vvp_net_ptr_t,तार्किक)
{
      प्रतिज्ञा(0);
}

शून्य automatic_signal_base::release_pv(vvp_net_ptr_t,अचिन्हित,अचिन्हित,तार्किक)
{
      प्रतिज्ञा(0);
}

अचिन्हित automatic_signal_base::filter_size() अचर
{
      प्रतिज्ञा(0);
      फर्काऊ(0);
}
शून्य automatic_signal_base::force_fil_vec4(अचर vvp_vector4_t&, अचर vvp_vector2_t&)
{
      प्रतिज्ञा(0);
}
शून्य automatic_signal_base::force_fil_vec8(अचर vvp_vector8_t&, अचर vvp_vector2_t&)
{
      प्रतिज्ञा(0);
}
शून्य automatic_signal_base::force_fil_real(द्विगुण, अचर vvp_vector2_t&)
{
      प्रतिज्ञा(0);
}
शून्य automatic_signal_base::get_मान(संरचना t_vpi_मान*)
{
      प्रतिज्ञा(0);
}

अचर vvp_vector4_t& vvp_fun_signal4_sa::vec4_unfiltered_मान() अचर
{
      फर्काऊ bits4_;
}

vvp_fun_signal4_aa::vvp_fun_signal4_aa(अचिन्हित wid, vvp_bit4_t init)
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
शून्य vvp_fun_signal4_aa::recv_vec4(vvp_net_ptr_t ptr, अचर vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      प्रतिज्ञा(ptr.port() == 0);
      प्रतिज्ञा(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      यदि (!bits4->eeq(bit)) {
            *bits4 = bit;
            ptr.ptr()->send_vec4(*bits4, context);
      }
}

शून्य vvp_fun_signal4_aa::recv_vec4_pv(vvp_net_ptr_t ptr, अचर vvp_vector4_t&bit,
				      अचिन्हित base, अचिन्हित vwid, vvp_context_t context)
{
      प्रतिज्ञा(ptr.port() == 0);
      प्रतिज्ञा(size_ == vwid);
      प्रतिज्ञा(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      अचिन्हित wid = bit.size();
      लागि (अचिन्हित idx = 0 ;  idx < wid ;  idx += 1) {
            यदि (base+idx >= bits4->size()) रोक;
            bits4->set_bit(base+idx, bit.value(idx));
      }
      ptr.ptr()->send_vec4(*bits4, context);
}

अचिन्हित vvp_fun_signal4_aa::मान_size() अचर
{
      फर्काऊ size_;
}

vvp_bit4_t vvp_fun_signal4_aa::value(अचिन्हित idx) अचर
{
      अचर vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      फर्काऊ bits4->value(idx);
}

vvp_scalar_t vvp_fun_signal4_aa::scalar_मान(अचिन्हित idx) अचर
{
      अचर vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      फर्काऊ vvp_scalar_t(bits4->value(idx), 6, 6);
}

शून्य vvp_fun_signal4_aa::vec4_मान(vvp_vector4_t&val) अचर
{
      अचर vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      val = *bits4;
}

अचर vvp_vector4_t&vvp_fun_signal4_aa::vec4_unfiltered_मान() अचर
{
      अचर vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      फर्काऊ *bits4;
}

शून्य vvp_fun_signal4_aa::चालक delete(शून्य*)
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
      फर्काऊ स_भेद(&a, &b, परिमाण a) == 0;
}

vvp_fun_signal_real_sa::vvp_fun_signal_real_sa()
{
      bits_ = 0.0;
}

द्विगुण vvp_fun_signal_real_sa::real_unfiltered_मान() अचर
{
      फर्काऊ bits_;
}

शून्य vvp_fun_signal_real_sa::recv_real(vvp_net_ptr_t ptr, द्विगुण bit,
                                       vvp_context_t)
{
      विकल्प (ptr.port()) {
	  पक्ष 0:
	    यदि (!continuous_assign_active_) {
                  यदि (needs_init_ || !bits_equal(bits_, bit)) {
			bits_ = bit;
			needs_init_ = असत्य;
			ptr.ptr()->send_real(bit, 0);
		  }
	    }
	    रोक;

	  पक्ष 1: // Continuous assign value
	    continuous_assign_active_ = सत्य;
	    bits_ = bit;
	    ptr.ptr()->send_real(bit, 0);
	    रोक;

	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "Unsupported port type %u.\n", ptr.port());
	    प्रतिज्ञा(0);
	    रोक;
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

द्विगुण vvp_fun_signal_real_aa::real_unfiltered_मान() अचर
{
      अचर द्विगुण*bits = static_cast<द्विगुण*>
            (vthread_get_rd_context_item(context_idx_));

      फर्काऊ *bits;
}

द्विगुण vvp_fun_signal_real_aa::real_मान() अचर
{
      फर्काऊ real_unfiltered_मान();
}

शून्य vvp_fun_signal_real_aa::recv_real(vvp_net_ptr_t ptr, द्विगुण bit,
                                       vvp_context_t context)
{
      प्रतिज्ञा(ptr.port() == 0);
      प्रतिज्ञा(context);

      द्विगुण*bits = static_cast<द्विगुण*>
            (vvp_get_context_item(context, context_idx_));

      यदि (!bits_equal(*bits,bit)) {
            *bits = bit;
            ptr.ptr()->send_real(bit, context);
      }
}

अचिन्हित vvp_fun_signal_real_aa::मान_size() अचर
{
      प्रतिज्ञा(0);
      फर्काऊ 1;
}

vvp_bit4_t vvp_fun_signal_real_aa::value(अचिन्हित) अचर
{
      प्रतिज्ञा(0);
      फर्काऊ BIT4_X;
}

vvp_scalar_t vvp_fun_signal_real_aa::scalar_मान(अचिन्हित) अचर
{
      प्रतिज्ञा(0);
      फर्काऊ vvp_scalar_t();
}

शून्य vvp_fun_signal_real_aa::vec4_मान(vvp_vector4_t&) अचर
{
      प्रतिज्ञा(0);
}

शून्य* vvp_fun_signal_real_aa::चालक नूतन(std::माप_प्रकार size)
{
      फर्काऊ vvp_net_fun_t::heap_.alloc(size);
}

शून्य vvp_fun_signal_real_aa::चालक delete(शून्य*)
{
      प्रतिज्ञा(0);
}


vvp_fun_signal_string_sa::vvp_fun_signal_string_sa()
{
}

शून्य vvp_fun_signal_string_sa::recv_string(vvp_net_ptr_t ptr, अचर std::string&bit,
					   vvp_context_t)
{
      प्रतिज्ञा(ptr.port() == 0);

      यदि (needs_init_ || मान_2 != bit) {
	    मान_2 = bit;
	    needs_init_ = असत्य;

	    ptr.ptr()->send_string(bit, 0);
      }
}

अचर string& vvp_fun_signal_string_sa::get_string() अचर
{
      फर्काऊ मान_2;
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

शून्य vvp_fun_signal_string_aa::recv_string(vvp_net_ptr_t ptr, अचर std::string&bit, vvp_context_t context)
{
      प्रतिज्ञा(ptr.port() == 0);
      प्रतिज्ञा(context);

      string*bits = static_cast<std::string*>
	    (vvp_get_context_item(context, context_idx_));

      यदि (*bits != bit) {
	    *bits = bit;
	    ptr.ptr()->send_string(bit, context);
      }
}

अचिन्हित vvp_fun_signal_string_aa::मान_size() अचर
{
      प्रतिज्ञा(0);
      फर्काऊ 1;
}

vvp_bit4_t vvp_fun_signal_string_aa::value(अचिन्हित) अचर
{
      प्रतिज्ञा(0);
      फर्काऊ BIT4_X;
}

vvp_scalar_t vvp_fun_signal_string_aa::scalar_मान(अचिन्हित) अचर
{
      प्रतिज्ञा(0);
      फर्काऊ vvp_scalar_t();
}

शून्य vvp_fun_signal_string_aa::vec4_मान(vvp_vector4_t&) अचर
{
      प्रतिज्ञा(0);
}

द्विगुण vvp_fun_signal_string_aa::real_मान() अचर
{
      प्रतिज्ञा(0);
      फर्काऊ 0.0;
}

अचर std::string& vvp_fun_signal_string_aa::get_string() अचर
{
      अचर string*bits = static_cast<std::string*>
            (vthread_get_rd_context_item(context_idx_));

      फर्काऊ *bits;
}

शून्य* vvp_fun_signal_string_aa::चालक नूतन(std::माप_प्रकार size)
{
      फर्काऊ vvp_net_fun_t::heap_.alloc(size);
}

शून्य vvp_fun_signal_string_aa::चालक delete(शून्य*)
{
      प्रतिज्ञा(0);
}

  /* OBJECT signals */

vvp_fun_signal_object_sa::vvp_fun_signal_object_sa(अचिन्हित size)
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

      यदि (needs_init_ || मान_2 != bit) {
	    मान_2 = bit;
	    needs_init_ = असत्य;

	    ptr.ptr()->send_object(bit, 0);
      }
}

vvp_object_t vvp_fun_signal_object_sa::get_object() अचर
{
      फर्काऊ मान_2;
}

vvp_fun_signal_object_aa::vvp_fun_signal_object_aa(अचिन्हित size)
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

vvp_object_t vvp_fun_signal_object_aa::get_object() अचर
{
      अचर vvp_object_t*bits = static_cast<vvp_object_t*>
	    (vthread_get_rd_context_item(context_idx_));
      फर्काऊ *bits;
}

शून्य vvp_fun_signal_object_aa::recv_object(vvp_net_ptr_t ptr, vvp_object_t bit,
					   vvp_context_t context)
{
      प्रतिज्ञा(ptr.port() == 0);
      प्रतिज्ञा(context);

      vvp_object_t*bits = static_cast<vvp_object_t*>
	    (vvp_get_context_item(context, context_idx_));

      यदि (*bits != bit) {
	    *bits = bit;
	    ptr.ptr()->send_object(bit, context);
      }
}

अचिन्हित vvp_fun_signal_object_aa::मान_size() अचर
{
      फर्काऊ 1;
}

vvp_bit4_t vvp_fun_signal_object_aa::value(अचिन्हित) अचर
{
      प्रतिज्ञा(0);
      फर्काऊ BIT4_X;
}

vvp_scalar_t vvp_fun_signal_object_aa::scalar_मान(अचिन्हित) अचर
{
      प्रतिज्ञा(0);
      फर्काऊ vvp_scalar_t();
}

शून्य vvp_fun_signal_object_aa::vec4_मान(vvp_vector4_t&) अचर
{
      प्रतिज्ञा(0);
}

शून्य* vvp_fun_signal_object_aa::चालक नूतन(std::माप_प्रकार size)
{
      फर्काऊ vvp_net_fun_t::heap_.alloc(size);
}

शून्य vvp_fun_signal_object_aa::चालक delete(शून्य*)
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

शून्य vvp_fun_force::recv_vec4(vvp_net_ptr_t ptr, अचर vvp_vector4_t&bit,
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

vvp_bit4_t vvp_wire_base::driven_मान(अचिन्हित) अचर
{
      प्रतिज्ञा(0);
      फर्काऊ BIT4_X;
}

तार्किक vvp_wire_base::is_forced(अचिन्हित) अचर
{
      प्रतिज्ञा(0);
      फर्काऊ असत्य;
}

vvp_wire_vec4::vvp_wire_vec4(अचिन्हित wid, vvp_bit4_t init)
: bits4_(wid, init)
{
      needs_init_ = सत्य;
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec4(अचर vvp_vector4_t&bit, vvp_vector4_t&rep,
						 अचिन्हित base, अचिन्हित vwid)
{
	// Special case! the input bit is 0 wid. Interpret this as a
	// vector of BIT4_X to match the width of the bits4_ vector.
	// FIXME! This is a hack to work around some buggy gate
	// implementations! This should be removed!
      यदि (base==0 && vwid==0) {
	    vvp_vector4_t tmp (bits4_.size(), BIT4_X);
	    यदि (bits4_ .eeq(tmp) && !needs_init_) फर्काऊ STOP;
	    bits4_ = tmp;
	    needs_init_ = असत्य;
	    फर्काऊ filter_mask_(tmp, force4_, rep, 0);
      }

      यदि (vwid != bits4_.size()) {
	    cerr << "Internal error: Input vector expected width="
		 << bits4_.size() << ", got "
		 << "bit=" << bit << ", base=" << base << ", vwid=" << vwid
		 << endl;
      }
      प्रतिज्ञा(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      यदि (base==0 && bit.size()==vwid) {
	    यदि (bits4_ .eeq( bit ) && !needs_init_) फर्काऊ STOP;
	    bits4_ = bit;
      } अन्यथा {
	    तार्किक rc = bits4_.set_vec(base, bit);
	    यदि (rc == असत्य && !needs_init_) फर्काऊ STOP;
      }

      needs_init_ = असत्य;
      फर्काऊ filter_mask_(bit, force4_, rep, base);
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec8(अचर vvp_vector8_t&bit,
                                                 vvp_vector8_t&rep,
                                                 अचिन्हित base,
                                                 अचिन्हित vwid)
{
      प्रतिज्ञा(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      vvp_vector4_t bit4 (reduce4(bit));
      यदि (base==0 && bit4.size()==vwid) {
	    यदि (bits4_ .eeq( bit4 ) && !needs_init_) फर्काऊ STOP;
	    bits4_ = bit4;
      } अन्यथा {
	    तार्किक rc = bits4_.set_vec(base, bit4);
	    यदि (rc == असत्य && !needs_init_) फर्काऊ STOP;
      }

      needs_init_ = असत्य;
      फर्काऊ filter_mask_(bit, vvp_vector8_t(force4_,6,6), rep, base);
}

अचिन्हित vvp_wire_vec4::filter_size() अचर
{
      फर्काऊ bits4_.size();
}

शून्य vvp_wire_vec4::force_fil_vec4(अचर vvp_vector4_t&val, अचर vvp_vector2_t&mask)
{
      force_mask(mask);

      यदि (force4_.size() == 0) {
	    force4_ = val;
      } अन्यथा {
	    लागि (अचिन्हित idx = 0; idx < mask.size() ; idx += 1) {
		  यदि (mask.value(idx) == 0)
			जारी;

		  force4_.set_bit(idx, val.value(idx));
	    }
      }
      run_vpi_callbacks();
}

शून्य vvp_wire_vec4::force_fil_vec8(अचर vvp_vector8_t&, अचर vvp_vector2_t&)
{
      प्रतिज्ञा(0);
}

शून्य vvp_wire_vec4::force_fil_real(द्विगुण, अचर vvp_vector2_t&)
{
      प्रतिज्ञा(0);
}

शून्य vvp_wire_vec4::release(vvp_net_ptr_t ptr, तार्किक net_flag)
{
      vvp_vector2_t mask (vvp_vector2_t::FILL1, bits4_.size());
      यदि (net_flag) {
	      // Wires revert to their unforced value after release.
            release_mask(mask);
	    needs_init_ = ! force4_ .eeq(bits4_);
	    ptr.ptr()->send_vec4(bits4_, 0);
	    run_vpi_callbacks();
      } अन्यथा {
	      // Variables keep the current value.
	    vvp_vector4_t res (bits4_.size());
	    लागि (अचिन्हित idx=0; idx<bits4_.size(); idx += 1)
		  res.set_bit(idx,value(idx));
            release_mask(mask);
	    ptr.ptr()->fun->recv_vec4(ptr, res, 0);
      }
}

शून्य vvp_wire_vec4::release_pv(vvp_net_ptr_t ptr, अचिन्हित base, अचिन्हित wid, तार्किक net_flag)
{
      प्रतिज्ञा(bits4_.size() >= base + wid);

      vvp_vector2_t mask (vvp_vector2_t::FILL0, bits4_.size());
      लागि (अचिन्हित idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      यदि (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_mask(mask);
	    needs_init_ = ! force4_.subvalue(base,wid) .eeq(bits4_.subvalue(base,wid));
	    ptr.ptr()->send_vec4_pv(bits4_.subvalue(base,wid),
				    base, bits4_.size(), 0);
	    run_vpi_callbacks();
      } अन्यथा {
	      // Variables keep the current value.
	    vvp_vector4_t res (wid);
	    लागि (अचिन्हित idx=0; idx<wid; idx += 1)
		  res.set_bit(idx,value(base+idx));
	    release_mask(mask);
	    ptr.ptr()->fun->recv_vec4_pv(ptr, res, base, bits4_.size(), 0);
      }
}

अचिन्हित vvp_wire_vec4::मान_size() अचर
{
      फर्काऊ bits4_.size();
}

vvp_bit4_t vvp_wire_vec4::filtered_मान(अचिन्हित idx) अचर
{
      यदि (test_force_mask(idx))
	    फर्काऊ force4_.value(idx);
      अन्यथा
	    फर्काऊ bits4_.value(idx);
}

vvp_bit4_t vvp_wire_vec4::value(अचिन्हित idx) अचर
{
      फर्काऊ filtered_मान(idx);
}

vvp_scalar_t vvp_wire_vec4::scalar_मान(अचिन्हित idx) अचर
{
      फर्काऊ vvp_scalar_t(value(idx),6,6);
}

शून्य vvp_wire_vec4::vec4_मान(vvp_vector4_t&val) अचर
{
      val = bits4_;
      यदि (test_force_mask_is_zero())
	    फर्काऊ;

      लागि (अचिन्हित idx = 0 ; idx < bits4_.size() ; idx += 1)
	    val.set_bit(idx, filtered_मान(idx));
}

vvp_bit4_t vvp_wire_vec4::driven_मान(अचिन्हित idx) अचर
{
      फर्काऊ bits4_.value(idx);
}

तार्किक vvp_wire_vec4::is_forced(अचिन्हित idx) अचर
{
      फर्काऊ test_force_mask(idx);
}

vvp_wire_vec8::vvp_wire_vec8(अचिन्हित wid)
: bits8_(wid)
{
      needs_init_ = सत्य;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec4(अचर vvp_vector4_t&bit,
                                                 vvp_vector4_t&rep,
                                                 अचिन्हित base,
                                                 अचिन्हित vwid)
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
      यदि (rc == REPL)
	    rep = reduce4(rep8);

      needs_init_ = असत्य;
      फर्काऊ rc;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec8(अचर vvp_vector8_t&bit, vvp_vector8_t&rep, अचिन्हित base, अचिन्हित vwid)
{
      प्रतिज्ञा(vwid == bits8_.size());
	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      यदि (base==0 && bit.size()==vwid) {
	    यदि (bits8_.eeq(bit) && !needs_init_) फर्काऊ STOP;
	    bits8_ = bit;
      } अन्यथा {
	    यदि (bits8_.subvalue(base, bit.size()).eeq(bit) &&
		!needs_init_) {
		  फर्काऊ STOP;
	    }
	    यदि (bits8_.size() == 0)
		  bits8_ = vvp_vector8_t(vwid);
	    प्रतिज्ञा(bits8_.size() == vwid);
	    bits8_.set_vec(base, bit);
      }
      needs_init_ = असत्य;
      फर्काऊ filter_mask_(bit, force8_, rep, base);
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_आगत_vec8(अचर vvp_vector8_t&bit, vvp_vector8_t&rep) अचर
{
      फर्काऊ filter_आगत_mask(bit, force8_, rep);
}

अचिन्हित vvp_wire_vec8::filter_size() अचर
{
      फर्काऊ bits8_.size();
}

शून्य vvp_wire_vec8::force_fil_vec4(अचर vvp_vector4_t&val, अचर vvp_vector2_t&mask)
{
      force_fil_vec8(vvp_vector8_t(val,6,6), mask);
}

शून्य vvp_wire_vec8::force_fil_vec8(अचर vvp_vector8_t&val, अचर vvp_vector2_t&mask)
{
      force_mask(mask);

      यदि (force8_.size() == 0) {
	    force8_ = val;
      } अन्यथा {
	    लागि (अचिन्हित idx = 0; idx < mask.size() ; idx += 1) {
		  यदि (mask.value(idx) == 0)
			जारी;

		  force8_.set_bit(idx, val.value(idx));
	    }
      }
      run_vpi_callbacks();
}

शून्य vvp_wire_vec8::force_fil_real(द्विगुण, अचर vvp_vector2_t&)
{
      प्रतिज्ञा(0);
}

शून्य vvp_wire_vec8::release(vvp_net_ptr_t ptr, तार्किक net_flag)
{
	// Wires revert to their unforced value after release.
      vvp_vector2_t mask (vvp_vector2_t::FILL1, bits8_.size());
      release_mask(mask);
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

शून्य vvp_wire_vec8::release_pv(vvp_net_ptr_t ptr, अचिन्हित base, अचिन्हित wid, तार्किक net_flag)
{
      प्रतिज्ञा(bits8_.size() >= base + wid);

      vvp_vector2_t mask (vvp_vector2_t::FILL0, bits8_.size());
      लागि (अचिन्हित idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      release_mask(mask);

      यदि (net_flag) {
	    needs_init_ = !force8_.subvalue(base,wid) .eeq((bits8_.subvalue(base,wid)));
	    ptr.ptr()->send_vec8_pv(bits8_.subvalue(base,wid),
				    base, bits8_.size());
	    run_vpi_callbacks();
      } अन्यथा {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    प्रतिज्ञा(0);
//	    ptr.ptr()->fun->recv_vec8_pv(ptr, force8_.subvalue(base,wid),
//					 base, force8_.size());
      }
}

अचिन्हित vvp_wire_vec8::मान_size() अचर
{
      फर्काऊ bits8_.size();
}

vvp_scalar_t vvp_wire_vec8::filtered_मान(अचिन्हित idx) अचर
{
      यदि (test_force_mask(idx))
	    फर्काऊ force8_.value(idx);
      अन्यथा
	    फर्काऊ bits8_.value(idx);
}

vvp_bit4_t vvp_wire_vec8::value(अचिन्हित idx) अचर
{
      फर्काऊ filtered_मान(idx).value();
}

vvp_scalar_t vvp_wire_vec8::scalar_मान(अचिन्हित idx) अचर
{
      फर्काऊ filtered_मान(idx);
}

vvp_vector8_t vvp_wire_vec8::vec8_मान() अचर
{
      vvp_vector8_t tmp = bits8_;
      लागि (अचिन्हित idx = 0 ; idx < bits8_.size() ; idx += 1)
	    tmp.set_bit(idx, filtered_मान(idx));
      फर्काऊ tmp;
}

शून्य vvp_wire_vec8::vec4_मान(vvp_vector4_t&val) अचर
{
      val = reduce4(vec8_मान());
}

vvp_bit4_t vvp_wire_vec8::driven_मान(अचिन्हित idx) अचर
{
      फर्काऊ bits8_.value(idx).value();
}

तार्किक vvp_wire_vec8::is_forced(अचिन्हित idx) अचर
{
      फर्काऊ test_force_mask(idx);
}

vvp_wire_real::vvp_wire_real()
: bit_(0.0), force_(0.0)
{
}

vvp_net_fil_t::prop_t vvp_wire_real::filter_real(द्विगुण&bit)
{
      यदि (bits_equal(bit_, bit) && !needs_init_) फर्काऊ STOP;
      bit_ = bit;
      needs_init_ = असत्य;
      फर्काऊ filter_mask_(bit, force_);
}

अचिन्हित vvp_wire_real::filter_size() अचर
{
      प्रतिज्ञा(0);
      फर्काऊ 0;
}

शून्य vvp_wire_real::force_fil_vec4(अचर vvp_vector4_t&, अचर vvp_vector2_t&)
{
      प्रतिज्ञा(0);
}

शून्य vvp_wire_real::force_fil_vec8(अचर vvp_vector8_t&, अचर vvp_vector2_t&)
{
      प्रतिज्ञा(0);
}

शून्य vvp_wire_real::force_fil_real(द्विगुण val, अचर vvp_vector2_t&mask)
{
      force_mask(mask);
      यदि (mask.value(0))
	    force_ = val;

      run_vpi_callbacks();
}

शून्य vvp_wire_real::release(vvp_net_ptr_t ptr, तार्किक net_flag)
{
      vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
      यदि (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_mask(mask);
	    needs_init_ = !bits_equal(force_, bit_);
	    ptr.ptr()->send_real(bit_, 0);
      } अन्यथा {
	      // Variables keep the current value.
	    द्विगुण res =  real_मान();
	    release_mask(mask);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
}

शून्य vvp_wire_real::release_pv(vvp_net_ptr_t, अचिन्हित, अचिन्हित, तार्किक)
{
      प्रतिज्ञा(0);
#यदि 0
	// A real is a single value. If for some reason this part release
	// can happen the following code should work correctly (requires
	// a base of 0 and a width of 1).
      vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
      प्रतिज्ञा(base == 0 && wid == 1);

      यदि (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_mask(mask);
	    ptr.ptr()->send_real(bit_, 0);
      } अन्यथा {
	      // Variables keep the current value.
	    द्विगुण res =  real_मान();
	    release_mask(mask);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
#endif
}

अचिन्हित vvp_wire_real::मान_size() अचर
{
      प्रतिज्ञा(0);
      फर्काऊ 1;
}

vvp_bit4_t vvp_wire_real::value(अचिन्हित) अचर
{
      प्रतिज्ञा(0);
      फर्काऊ BIT4_X;
}

vvp_scalar_t vvp_wire_real::scalar_मान(अचिन्हित) अचर
{
      प्रतिज्ञा(0);
      फर्काऊ vvp_scalar_t();
}

शून्य vvp_wire_real::vec4_मान(vvp_vector4_t&) अचर
{
      प्रतिज्ञा(0);
}

द्विगुण vvp_wire_real::real_मान() अचर
{
      यदि (test_force_mask(0))
	    फर्काऊ force_;
      अन्यथा
	    फर्काऊ bit_;
}

#यदि 0
vvp_wire_string::vvp_wire_string()
{
}

अचिन्हित vvp_wire_string::filter_size() अचर
{
      प्रतिज्ञा(0);
      फर्काऊ 0;
}

शून्य vvp_wire_string::force_fil_vec4(अचर vvp_vector4_t&, अचर vvp_vector2_t&)
{
      प्रतिज्ञा(0);
}
शून्य vvp_wire_string::force_fil_vec8(अचर vvp_vector8_t&, अचर vvp_vector2_t&)
{
      प्रतिज्ञा(0);
}
शून्य vvp_wire_string::force_fil_real(द्विगुण, अचर vvp_vector2_t&)
{
      प्रतिज्ञा(0);
}

शून्य vvp_wire_string::release(vvp_net_ptr_t ptr, तार्किक net_flag)
{
      प्रतिज्ञा(0);
}

शून्य vvp_wire_string::release_pv(vvp_net_ptr_t, अचिन्हित, अचिन्हित, तार्किक)
{
      प्रतिज्ञा(0);
}

अचिन्हित vvp_wire_string::मान_size() अचर
{
      प्रतिज्ञा(0);
      फर्काऊ 1;
}

vvp_bit4_t vvp_wire_string::value(अचिन्हित) अचर
{
      प्रतिज्ञा(0);
      फर्काऊ BIT4_X;
}

vvp_scalar_t vvp_wire_string::scalar_मान(अचिन्हित) अचर
{
      प्रतिज्ञा(0);
      फर्काऊ vvp_scalar_t();
}

शून्य vvp_wire_string::vec4_मान(vvp_vector4_t&) अचर
{
      प्रतिज्ञा(0);
}

द्विगुण vvp_wire_string::real_मान() अचर
{
      प्रतिज्ञा(0);
      फर्काऊ 0.0;
}
#endif
