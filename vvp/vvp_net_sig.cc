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

using நாமாகாஶ std;

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
template <வகுப்பு T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_mask_(மாறிலி T&val, மாறிலி T&force, T&filter, unsigned base)
{
      எனில் (!test_force_mask_is_zero()) {
	      // Some bits are being forced. Go through the
	      // force_mask_ and force value to see which bits are
	      // propagated and which are kept from the forced
	      // value. Update the filter with the filtered result and
	      // return REPL to indicate that some bits have changed,
	      // or STOP if no bits change.
	    தருக்க propagate_flag = force_propagate_;
	    force_propagate_ = பொய்;
	    ப்ரதிஜ்ஞா(force_mask_.size() == force.size());
	    ப்ரதிஜ்ஞா((base+val.size()) <= force_mask_.size());

	    filter = val;
	    க்கு (unsigned idx = 0 ; idx < val.size() ; idx += 1) {
		  எனில் (force_mask_.value(base+idx))
			filter.set_bit(idx, force.value(base+idx));
		  இல்லையெனில்
			propagate_flag = உண்மை;
	    }

	    எனில் (propagate_flag) {
		  run_vpi_callbacks();
		  திரும்பு REPL;
	    } இல்லையெனில் {
		  திரும்பு STOP;
	    }

      } இல்லையெனில் {
	    run_vpi_callbacks();
	    திரும்பு PROP;
      }
}

template <வகுப்பு T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_mask_(T&val, T force)
{

      எனில் (test_force_mask(0)) {
	    val = force;
	    run_vpi_callbacks();
	    திரும்பு REPL;
      }
      run_vpi_callbacks();
      திரும்பு PROP;
}

template <வகுப்பு T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_உள்ளீடு_mask(மாறிலி T&val, மாறிலி T&force, T&rep) மாறிலி
{
      எனில் (test_force_mask_is_zero())
	    திரும்பு PROP;

      ப்ரதிஜ்ஞா(force_mask_.size() == force.size());

      rep = val;
      க்கு (unsigned idx = 0 ; idx < val.size() ; idx += 1) {
	    எனில் (force_mask_.value(idx))
		  rep.set_bit(idx, force.value(idx));
      }

      திரும்பு REPL;
}

vvp_signal_மதிப்பு::~vvp_signal_மதிப்பு()
{
}

இரட்டை vvp_signal_மதிப்பு::real_மதிப்பு() மாறிலி
{
      ப்ரதிஜ்ஞா(0);
      திரும்பு 0;
}

வெறுமை vvp_net_t::force_vec4(மாறிலி vvp_vector4_t&val, மாறிலி vvp_vector2_t&mask)
{
      ப்ரதிஜ்ஞா(fil);
      fil->force_fil_vec4(val, mask);
      fun->force_flag(பொய்);
      vvp_send_vec4(out_, val, 0);
}

வெறுமை vvp_net_t::force_vec8(மாறிலி vvp_vector8_t&val, மாறிலி vvp_vector2_t&mask)
{
      ப்ரதிஜ்ஞா(fil);
      fil->force_fil_vec8(val, mask);
      fun->force_flag(பொய்);
      vvp_send_vec8(out_, val);
}

வெறுமை vvp_net_t::force_real(இரட்டை val, மாறிலி vvp_vector2_t&mask)
{
      ப்ரதிஜ்ஞா(fil);
      fil->force_fil_real(val, mask);
      fun->force_flag(பொய்);
      vvp_send_real(out_, val, 0);
}

/* **** vvp_fun_signal methods **** */

vvp_fun_signal_base::vvp_fun_signal_base()
{
      continuous_assign_active_ = பொய்;
      needs_init_ = உண்மை;
      cassign_link = 0;
      மொத்தம்_functors_sig += 1;
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
வெறுமை vvp_fun_signal4_sa::recv_vec4(vvp_net_ptr_t ptr, மாறிலி vvp_vector4_t&bit,
                                   vvp_context_t)
{
      தேர்வு (ptr.port()) {
	  நிலை 0: // Normal input (feed from net, or set from process)
	      /* If we don't have a continuous assign mask then just
		 copy the bits, otherwise we need to see if there are
		 any holes in the mask so we can set those bits. */
	    எனில் (assign_mask_.size() == 0) {
                  எனில் (needs_init_ || !bits4_.eeq(bit)) {
			ப்ரதிஜ்ஞா(bit.size() == bits4_.size());
			bits4_ = bit;
			needs_init_ = பொய்;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    } இல்லையெனில் {
		  தருக்க changed = பொய்;
		  ப்ரதிஜ்ஞா(bits4_.size() == assign_mask_.size());
		  க்கு (unsigned idx = 0 ;  idx < bit.size() ;  idx += 1) {
			எனில் (idx >= bits4_.size()) நிறுத்து;
			எனில் (assign_mask_.value(idx)) தொடர்;
			bits4_.set_bit(idx, bit.value(idx));
			changed = உண்மை;
		  }
		  எனில் (changed) {
			needs_init_ = பொய்;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    }
	    நிறுத்து;

	  நிலை 1: // Continuous assign value
	      // Handle the simple case of the linked source being wider
	      // than this signal. Note we don't yet support the case of
	      // the linked source being narrower than this signal, or
	      // the case of an expression being assigned.
	    bits4_ = coerce_to_width(bit, bits4_.size());
	    assign_mask_ = vvp_vector2_t(vvp_vector2_t::FILL1, bits4_.size());
	    ptr.ptr()->send_vec4(bits4_, 0);
	    நிறுத்து;

	  இயல்பு:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    ப்ரதிஜ்ஞா(0);
	    நிறுத்து;
      }
}

வெறுமை vvp_fun_signal4_sa::recv_vec8(vvp_net_ptr_t ptr, மாறிலி vvp_vector8_t&bit)
{
      recv_vec4(ptr, reduce4(bit), 0);
}

வெறுமை vvp_fun_signal4_sa::recv_vec4_pv(vvp_net_ptr_t ptr, மாறிலி vvp_vector4_t&bit,
				      unsigned base, unsigned vwid, vvp_context_t)
{
      ப்ரதிஜ்ஞா(bits4_.size() == vwid);
      unsigned wid = bit.size();

      தேர்வு (ptr.port()) {
	  நிலை 0: // Normal input
	    எனில் (assign_mask_.size() == 0) {
                  க்கு (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
			எனில் (base+idx >= bits4_.size()) நிறுத்து;
			bits4_.set_bit(base+idx, bit.value(idx));
		  }
		  needs_init_ = பொய்;
		  ptr.ptr()->send_vec4(bits4_,0);
	    } இல்லையெனில் {
		  தருக்க changed = பொய்;
		  ப்ரதிஜ்ஞா(bits4_.size() == assign_mask_.size());
		  க்கு (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
			எனில் (base+idx >= bits4_.size()) நிறுத்து;
			எனில் (assign_mask_.value(base+idx)) தொடர்;
			bits4_.set_bit(base+idx, bit.value(idx));
			changed = உண்மை;
		  }
		  எனில் (changed) {
			needs_init_ = பொய்;
			ptr.ptr()->send_vec4(bits4_,0);
		  }
	    }
	    நிறுத்து;

	  நிலை 1: // Continuous assign value
	    எனில் (assign_mask_.size() == 0)
		  assign_mask_ = vvp_vector2_t(vvp_vector2_t::FILL0, bits4_.size());
	    க்கு (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  எனில் (base+idx >= bits4_.size())
			நிறுத்து;
		  bits4_.set_bit(base+idx, bit.value(idx));
		  assign_mask_.set_bit(base+idx, 1);
	    }
	    ptr.ptr()->send_vec4(bits4_,0);
	    நிறுத்து;

	  இயல்பு:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    ப்ரதிஜ்ஞா(0);
	    நிறுத்து;
      }
}

வெறுமை vvp_fun_signal4_sa::recv_vec8_pv(vvp_net_ptr_t ptr, மாறிலி vvp_vector8_t&bit,
				      unsigned base, unsigned vwid)
{
      recv_vec4_pv(ptr, reduce4(bit), base, vwid, 0);
}

வெறுமை vvp_fun_signal_base::deassign()
{
      continuous_assign_active_ = பொய்;
      assign_mask_ = vvp_vector2_t();
}

வெறுமை vvp_fun_signal_base::deassign_pv(unsigned base, unsigned wid)
{
      க்கு (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    assign_mask_.set_bit(base+idx, 0);
      }

      எனில் (assign_mask_.is_zero()) {
	    assign_mask_ = vvp_vector2_t();
      }
}

வெறுமை automatic_signal_base::release(vvp_net_ptr_t,தருக்க)
{
      ப்ரதிஜ்ஞா(0);
}

வெறுமை automatic_signal_base::release_pv(vvp_net_ptr_t,unsigned,unsigned,தருக்க)
{
      ப்ரதிஜ்ஞா(0);
}

unsigned automatic_signal_base::filter_size() மாறிலி
{
      ப்ரதிஜ்ஞா(0);
      திரும்பு(0);
}
வெறுமை automatic_signal_base::force_fil_vec4(மாறிலி vvp_vector4_t&, மாறிலி vvp_vector2_t&)
{
      ப்ரதிஜ்ஞா(0);
}
வெறுமை automatic_signal_base::force_fil_vec8(மாறிலி vvp_vector8_t&, மாறிலி vvp_vector2_t&)
{
      ப்ரதிஜ்ஞா(0);
}
வெறுமை automatic_signal_base::force_fil_real(இரட்டை, மாறிலி vvp_vector2_t&)
{
      ப்ரதிஜ்ஞா(0);
}
வெறுமை automatic_signal_base::get_மதிப்பு(கட்டமைப்பு t_vpi_மதிப்பு*)
{
      ப்ரதிஜ்ஞா(0);
}

மாறிலி vvp_vector4_t& vvp_fun_signal4_sa::vec4_unfiltered_மதிப்பு() மாறிலி
{
      திரும்பு bits4_;
}

vvp_fun_signal4_aa::vvp_fun_signal4_aa(unsigned wid, vvp_bit4_t init)
{
      context_idx_ = vpip_add_item_to_context(இது, vpip_peek_context_scope());
      size_ = wid;
      init_ = init;
}

vvp_fun_signal4_aa::~vvp_fun_signal4_aa()
{
      ப்ரதிஜ்ஞா(0);
}

வெறுமை vvp_fun_signal4_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, புதிய vvp_vector4_t(size_, init_));
}

வெறுமை vvp_fun_signal4_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*bits = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      bits->fill_bits(init_);
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை vvp_fun_signal4_aa::free_instance(vvp_context_t context)
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
வெறுமை vvp_fun_signal4_aa::recv_vec4(vvp_net_ptr_t ptr, மாறிலி vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      ப்ரதிஜ்ஞா(ptr.port() == 0);
      ப்ரதிஜ்ஞா(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      எனில் (!bits4->eeq(bit)) {
            *bits4 = bit;
            ptr.ptr()->send_vec4(*bits4, context);
      }
}

வெறுமை vvp_fun_signal4_aa::recv_vec4_pv(vvp_net_ptr_t ptr, மாறிலி vvp_vector4_t&bit,
				      unsigned base, unsigned vwid, vvp_context_t context)
{
      ப்ரதிஜ்ஞா(ptr.port() == 0);
      ப்ரதிஜ்ஞா(size_ == vwid);
      ப்ரதிஜ்ஞா(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      unsigned wid = bit.size();
      க்கு (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
            எனில் (base+idx >= bits4->size()) நிறுத்து;
            bits4->set_bit(base+idx, bit.value(idx));
      }
      ptr.ptr()->send_vec4(*bits4, context);
}

unsigned vvp_fun_signal4_aa::மதிப்பு_size() மாறிலி
{
      திரும்பு size_;
}

vvp_bit4_t vvp_fun_signal4_aa::value(unsigned idx) மாறிலி
{
      மாறிலி vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      திரும்பு bits4->value(idx);
}

vvp_scalar_t vvp_fun_signal4_aa::scalar_மதிப்பு(unsigned idx) மாறிலி
{
      மாறிலி vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      திரும்பு vvp_scalar_t(bits4->value(idx), 6, 6);
}

வெறுமை vvp_fun_signal4_aa::vec4_மதிப்பு(vvp_vector4_t&val) மாறிலி
{
      மாறிலி vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      val = *bits4;
}

மாறிலி vvp_vector4_t&vvp_fun_signal4_aa::vec4_unfiltered_மதிப்பு() மாறிலி
{
      மாறிலி vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      திரும்பு *bits4;
}

வெறுமை vvp_fun_signal4_aa::operator delete(வெறுமை*)
{
      ப்ரதிஜ்ஞா(0);
}

/*
 * Testing for equality, we want a bitwise test instead of an
 * arithmetic test because we want to treat for example -0 different
 * from +0.
 */
தருக்க bits_equal(இரட்டை a, இரட்டை b)
{
      திரும்பு memcmp(&a, &b, அளவு a) == 0;
}

vvp_fun_signal_real_sa::vvp_fun_signal_real_sa()
{
      bits_ = 0.0;
}

இரட்டை vvp_fun_signal_real_sa::real_unfiltered_மதிப்பு() மாறிலி
{
      திரும்பு bits_;
}

வெறுமை vvp_fun_signal_real_sa::recv_real(vvp_net_ptr_t ptr, இரட்டை bit,
                                       vvp_context_t)
{
      தேர்வு (ptr.port()) {
	  நிலை 0:
	    எனில் (!continuous_assign_active_) {
                  எனில் (needs_init_ || !bits_equal(bits_, bit)) {
			bits_ = bit;
			needs_init_ = பொய்;
			ptr.ptr()->send_real(bit, 0);
		  }
	    }
	    நிறுத்து;

	  நிலை 1: // Continuous assign value
	    continuous_assign_active_ = உண்மை;
	    bits_ = bit;
	    ptr.ptr()->send_real(bit, 0);
	    நிறுத்து;

	  இயல்பு:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    ப்ரதிஜ்ஞா(0);
	    நிறுத்து;
      }
}

vvp_fun_signal_real_aa::vvp_fun_signal_real_aa()
{
      context_idx_ = vpip_add_item_to_context(இது, vpip_peek_context_scope());
}

vvp_fun_signal_real_aa::~vvp_fun_signal_real_aa()
{
      ப்ரதிஜ்ஞா(0);
}

வெறுமை vvp_fun_signal_real_aa::alloc_instance(vvp_context_t context)
{
      இரட்டை*bits = புதிய இரட்டை;
      vvp_set_context_item(context, context_idx_, bits);

      *bits = 0.0;
}

வெறுமை vvp_fun_signal_real_aa::reset_instance(vvp_context_t context)
{
      இரட்டை*bits = static_cast<இரட்டை*>
            (vvp_get_context_item(context, context_idx_));

      *bits = 0.0;
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை vvp_fun_signal_real_aa::free_instance(vvp_context_t context)
{
      இரட்டை*bits = static_cast<இரட்டை*>
            (vvp_get_context_item(context, context_idx_));
      delete bits;
}
#endif

இரட்டை vvp_fun_signal_real_aa::real_unfiltered_மதிப்பு() மாறிலி
{
      மாறிலி இரட்டை*bits = static_cast<இரட்டை*>
            (vthread_get_rd_context_item(context_idx_));

      திரும்பு *bits;
}

இரட்டை vvp_fun_signal_real_aa::real_மதிப்பு() மாறிலி
{
      திரும்பு real_unfiltered_மதிப்பு();
}

வெறுமை vvp_fun_signal_real_aa::recv_real(vvp_net_ptr_t ptr, இரட்டை bit,
                                       vvp_context_t context)
{
      ப்ரதிஜ்ஞா(ptr.port() == 0);
      ப்ரதிஜ்ஞா(context);

      இரட்டை*bits = static_cast<இரட்டை*>
            (vvp_get_context_item(context, context_idx_));

      எனில் (!bits_equal(*bits,bit)) {
            *bits = bit;
            ptr.ptr()->send_real(bit, context);
      }
}

unsigned vvp_fun_signal_real_aa::மதிப்பு_size() மாறிலி
{
      ப்ரதிஜ்ஞா(0);
      திரும்பு 1;
}

vvp_bit4_t vvp_fun_signal_real_aa::value(unsigned) மாறிலி
{
      ப்ரதிஜ்ஞா(0);
      திரும்பு BIT4_X;
}

vvp_scalar_t vvp_fun_signal_real_aa::scalar_மதிப்பு(unsigned) மாறிலி
{
      ப்ரதிஜ்ஞா(0);
      திரும்பு vvp_scalar_t();
}

வெறுமை vvp_fun_signal_real_aa::vec4_மதிப்பு(vvp_vector4_t&) மாறிலி
{
      ப்ரதிஜ்ஞா(0);
}

வெறுமை* vvp_fun_signal_real_aa::operator புதிய(std::size_t size)
{
      திரும்பு vvp_net_fun_t::heap_.alloc(size);
}

வெறுமை vvp_fun_signal_real_aa::operator delete(வெறுமை*)
{
      ப்ரதிஜ்ஞா(0);
}


vvp_fun_signal_string_sa::vvp_fun_signal_string_sa()
{
}

வெறுமை vvp_fun_signal_string_sa::recv_string(vvp_net_ptr_t ptr, மாறிலி std::string&bit,
					   vvp_context_t)
{
      ப்ரதிஜ்ஞா(ptr.port() == 0);

      எனில் (needs_init_ || மதிப்பு_2 != bit) {
	    மதிப்பு_2 = bit;
	    needs_init_ = பொய்;

	    ptr.ptr()->send_string(bit, 0);
      }
}

மாறிலி string& vvp_fun_signal_string_sa::get_string() மாறிலி
{
      திரும்பு மதிப்பு_2;
}

vvp_fun_signal_string_aa::vvp_fun_signal_string_aa()
{
      context_idx_ = vpip_add_item_to_context(இது, vpip_peek_context_scope());
}

vvp_fun_signal_string_aa::~vvp_fun_signal_string_aa()
{
      ப்ரதிஜ்ஞா(0);
}

வெறுமை vvp_fun_signal_string_aa::alloc_instance(vvp_context_t context)
{
      string*bits = புதிய std::string;
      vvp_set_context_item(context, context_idx_, bits);
      *bits = "";
}

வெறுமை vvp_fun_signal_string_aa::reset_instance(vvp_context_t context)
{
      string*bits = static_cast<std::string*>
	    (vvp_get_context_item(context, context_idx_));
      *bits = "";
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை vvp_fun_signal_string_aa::free_instance(vvp_context_t context)
{
      string*bits = static_cast<std::string*>
            (vvp_get_context_item(context, context_idx_));
      delete bits;
}
#endif

வெறுமை vvp_fun_signal_string_aa::recv_string(vvp_net_ptr_t ptr, மாறிலி std::string&bit, vvp_context_t context)
{
      ப்ரதிஜ்ஞா(ptr.port() == 0);
      ப்ரதிஜ்ஞா(context);

      string*bits = static_cast<std::string*>
	    (vvp_get_context_item(context, context_idx_));

      எனில் (*bits != bit) {
	    *bits = bit;
	    ptr.ptr()->send_string(bit, context);
      }
}

unsigned vvp_fun_signal_string_aa::மதிப்பு_size() மாறிலி
{
      ப்ரதிஜ்ஞா(0);
      திரும்பு 1;
}

vvp_bit4_t vvp_fun_signal_string_aa::value(unsigned) மாறிலி
{
      ப்ரதிஜ்ஞா(0);
      திரும்பு BIT4_X;
}

vvp_scalar_t vvp_fun_signal_string_aa::scalar_மதிப்பு(unsigned) மாறிலி
{
      ப்ரதிஜ்ஞா(0);
      திரும்பு vvp_scalar_t();
}

வெறுமை vvp_fun_signal_string_aa::vec4_மதிப்பு(vvp_vector4_t&) மாறிலி
{
      ப்ரதிஜ்ஞா(0);
}

இரட்டை vvp_fun_signal_string_aa::real_மதிப்பு() மாறிலி
{
      ப்ரதிஜ்ஞா(0);
      திரும்பு 0.0;
}

மாறிலி std::string& vvp_fun_signal_string_aa::get_string() மாறிலி
{
      மாறிலி string*bits = static_cast<std::string*>
            (vthread_get_rd_context_item(context_idx_));

      திரும்பு *bits;
}

வெறுமை* vvp_fun_signal_string_aa::operator புதிய(std::size_t size)
{
      திரும்பு vvp_net_fun_t::heap_.alloc(size);
}

வெறுமை vvp_fun_signal_string_aa::operator delete(வெறுமை*)
{
      ப்ரதிஜ்ஞா(0);
}

  /* OBJECT signals */

vvp_fun_signal_object_sa::vvp_fun_signal_object_sa(unsigned size)
: vvp_fun_signal_object(size)
{
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை vvp_fun_signal_object_aa::free_instance(vvp_context_t context)
{
      vvp_object_t*bits = static_cast<vvp_object_t*>
            (vvp_get_context_item(context, context_idx_));
      delete bits;
}
#endif

வெறுமை vvp_fun_signal_object_sa::recv_object(vvp_net_ptr_t ptr, vvp_object_t bit,
					   vvp_context_t)
{
      ப்ரதிஜ்ஞா(ptr.port() == 0);

      எனில் (needs_init_ || மதிப்பு_2 != bit) {
	    மதிப்பு_2 = bit;
	    needs_init_ = பொய்;

	    ptr.ptr()->send_object(bit, 0);
      }
}

vvp_object_t vvp_fun_signal_object_sa::get_object() மாறிலி
{
      திரும்பு மதிப்பு_2;
}

vvp_fun_signal_object_aa::vvp_fun_signal_object_aa(unsigned size)
: vvp_fun_signal_object(size)
{
      context_idx_ = vpip_add_item_to_context(இது, vpip_peek_context_scope());
}

vvp_fun_signal_object_aa::~vvp_fun_signal_object_aa()
{
      ப்ரதிஜ்ஞா(0);
}

வெறுமை vvp_fun_signal_object_aa::alloc_instance(vvp_context_t context)
{
      vvp_object_t*bits = புதிய vvp_object_t;
      vvp_set_context_item(context, context_idx_, bits);
      bits->reset();
}

வெறுமை vvp_fun_signal_object_aa::reset_instance(vvp_context_t context)
{
      vvp_object_t*bits = static_cast<vvp_object_t*>
	    (vvp_get_context_item(context, context_idx_));
      bits->reset();
}

vvp_object_t vvp_fun_signal_object_aa::get_object() மாறிலி
{
      மாறிலி vvp_object_t*bits = static_cast<vvp_object_t*>
	    (vthread_get_rd_context_item(context_idx_));
      திரும்பு *bits;
}

வெறுமை vvp_fun_signal_object_aa::recv_object(vvp_net_ptr_t ptr, vvp_object_t bit,
					   vvp_context_t context)
{
      ப்ரதிஜ்ஞா(ptr.port() == 0);
      ப்ரதிஜ்ஞா(context);

      vvp_object_t*bits = static_cast<vvp_object_t*>
	    (vvp_get_context_item(context, context_idx_));

      எனில் (*bits != bit) {
	    *bits = bit;
	    ptr.ptr()->send_object(bit, context);
      }
}

unsigned vvp_fun_signal_object_aa::மதிப்பு_size() மாறிலி
{
      திரும்பு 1;
}

vvp_bit4_t vvp_fun_signal_object_aa::value(unsigned) மாறிலி
{
      ப்ரதிஜ்ஞா(0);
      திரும்பு BIT4_X;
}

vvp_scalar_t vvp_fun_signal_object_aa::scalar_மதிப்பு(unsigned) மாறிலி
{
      ப்ரதிஜ்ஞா(0);
      திரும்பு vvp_scalar_t();
}

வெறுமை vvp_fun_signal_object_aa::vec4_மதிப்பு(vvp_vector4_t&) மாறிலி
{
      ப்ரதிஜ்ஞா(0);
}

வெறுமை* vvp_fun_signal_object_aa::operator புதிய(std::size_t size)
{
      திரும்பு vvp_net_fun_t::heap_.alloc(size);
}

வெறுமை vvp_fun_signal_object_aa::operator delete(வெறுமை*)
{
      ப்ரதிஜ்ஞா(0);
}

  /* **** */

vvp_fun_force::vvp_fun_force()
{
}

vvp_fun_force::~vvp_fun_force()
{
}

வெறுமை vvp_fun_force::recv_vec4(vvp_net_ptr_t ptr, மாறிலி vvp_vector4_t&bit,
			      vvp_context_t)
{
      ப்ரதிஜ்ஞா(ptr.port() == 0);
      vvp_net_t*net = ptr.ptr();

      vvp_net_t*dst = net->port[3].ptr();
      ப்ரதிஜ்ஞா(dst->fil);

      dst->force_vec4(coerce_to_width(bit, dst->fil->filter_size()), vvp_vector2_t(vvp_vector2_t::FILL1, dst->fil->filter_size()));
}

வெறுமை vvp_fun_force::recv_real(vvp_net_ptr_t ptr, இரட்டை bit, vvp_context_t)
{
      ப்ரதிஜ்ஞா(ptr.port() == 0);
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

vvp_bit4_t vvp_wire_base::driven_மதிப்பு(unsigned) மாறிலி
{
      ப்ரதிஜ்ஞா(0);
      திரும்பு BIT4_X;
}

தருக்க vvp_wire_base::is_forced(unsigned) மாறிலி
{
      ப்ரதிஜ்ஞா(0);
      திரும்பு பொய்;
}

vvp_wire_vec4::vvp_wire_vec4(unsigned wid, vvp_bit4_t init)
: bits4_(wid, init)
{
      needs_init_ = உண்மை;
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec4(மாறிலி vvp_vector4_t&bit, vvp_vector4_t&rep,
						 unsigned base, unsigned vwid)
{
	// Special case! the input bit is 0 wid. Interpret this as a
	// vector of BIT4_X to match the width of the bits4_ vector.
	// FIXME! This is a hack to work around some buggy gate
	// implementations! This should be removed!
      எனில் (base==0 && vwid==0) {
	    vvp_vector4_t tmp (bits4_.size(), BIT4_X);
	    எனில் (bits4_ .eeq(tmp) && !needs_init_) திரும்பு STOP;
	    bits4_ = tmp;
	    needs_init_ = பொய்;
	    திரும்பு filter_mask_(tmp, force4_, rep, 0);
      }

      எனில் (vwid != bits4_.size()) {
	    cerr << "Internal error: Input vector expected width="
		 << bits4_.size() << ", got "
		 << "bit=" << bit << ", base=" << base << ", vwid=" << vwid
		 << endl;
      }
      ப்ரதிஜ்ஞா(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      எனில் (base==0 && bit.size()==vwid) {
	    எனில் (bits4_ .eeq( bit ) && !needs_init_) திரும்பு STOP;
	    bits4_ = bit;
      } இல்லையெனில் {
	    தருக்க rc = bits4_.set_vec(base, bit);
	    எனில் (rc == பொய் && !needs_init_) திரும்பு STOP;
      }

      needs_init_ = பொய்;
      திரும்பு filter_mask_(bit, force4_, rep, base);
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec8(மாறிலி vvp_vector8_t&bit,
                                                 vvp_vector8_t&rep,
                                                 unsigned base,
                                                 unsigned vwid)
{
      ப்ரதிஜ்ஞா(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      vvp_vector4_t bit4 (reduce4(bit));
      எனில் (base==0 && bit4.size()==vwid) {
	    எனில் (bits4_ .eeq( bit4 ) && !needs_init_) திரும்பு STOP;
	    bits4_ = bit4;
      } இல்லையெனில் {
	    தருக்க rc = bits4_.set_vec(base, bit4);
	    எனில் (rc == பொய் && !needs_init_) திரும்பு STOP;
      }

      needs_init_ = பொய்;
      திரும்பு filter_mask_(bit, vvp_vector8_t(force4_,6,6), rep, base);
}

unsigned vvp_wire_vec4::filter_size() மாறிலி
{
      திரும்பு bits4_.size();
}

வெறுமை vvp_wire_vec4::force_fil_vec4(மாறிலி vvp_vector4_t&val, மாறிலி vvp_vector2_t&mask)
{
      force_mask(mask);

      எனில் (force4_.size() == 0) {
	    force4_ = val;
      } இல்லையெனில் {
	    க்கு (unsigned idx = 0; idx < mask.size() ; idx += 1) {
		  எனில் (mask.value(idx) == 0)
			தொடர்;

		  force4_.set_bit(idx, val.value(idx));
	    }
      }
      run_vpi_callbacks();
}

வெறுமை vvp_wire_vec4::force_fil_vec8(மாறிலி vvp_vector8_t&, மாறிலி vvp_vector2_t&)
{
      ப்ரதிஜ்ஞா(0);
}

வெறுமை vvp_wire_vec4::force_fil_real(இரட்டை, மாறிலி vvp_vector2_t&)
{
      ப்ரதிஜ்ஞா(0);
}

வெறுமை vvp_wire_vec4::release(vvp_net_ptr_t ptr, தருக்க net_flag)
{
      vvp_vector2_t mask (vvp_vector2_t::FILL1, bits4_.size());
      எனில் (net_flag) {
	      // Wires revert to their unforced value after release.
            release_mask(mask);
	    needs_init_ = ! force4_ .eeq(bits4_);
	    ptr.ptr()->send_vec4(bits4_, 0);
	    run_vpi_callbacks();
      } இல்லையெனில் {
	      // Variables keep the current value.
	    vvp_vector4_t res (bits4_.size());
	    க்கு (unsigned idx=0; idx<bits4_.size(); idx += 1)
		  res.set_bit(idx,value(idx));
            release_mask(mask);
	    ptr.ptr()->fun->recv_vec4(ptr, res, 0);
      }
}

வெறுமை vvp_wire_vec4::release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, தருக்க net_flag)
{
      ப்ரதிஜ்ஞா(bits4_.size() >= base + wid);

      vvp_vector2_t mask (vvp_vector2_t::FILL0, bits4_.size());
      க்கு (unsigned idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      எனில் (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_mask(mask);
	    needs_init_ = ! force4_.subvalue(base,wid) .eeq(bits4_.subvalue(base,wid));
	    ptr.ptr()->send_vec4_pv(bits4_.subvalue(base,wid),
				    base, bits4_.size(), 0);
	    run_vpi_callbacks();
      } இல்லையெனில் {
	      // Variables keep the current value.
	    vvp_vector4_t res (wid);
	    க்கு (unsigned idx=0; idx<wid; idx += 1)
		  res.set_bit(idx,value(base+idx));
	    release_mask(mask);
	    ptr.ptr()->fun->recv_vec4_pv(ptr, res, base, bits4_.size(), 0);
      }
}

unsigned vvp_wire_vec4::மதிப்பு_size() மாறிலி
{
      திரும்பு bits4_.size();
}

vvp_bit4_t vvp_wire_vec4::filtered_மதிப்பு(unsigned idx) மாறிலி
{
      எனில் (test_force_mask(idx))
	    திரும்பு force4_.value(idx);
      இல்லையெனில்
	    திரும்பு bits4_.value(idx);
}

vvp_bit4_t vvp_wire_vec4::value(unsigned idx) மாறிலி
{
      திரும்பு filtered_மதிப்பு(idx);
}

vvp_scalar_t vvp_wire_vec4::scalar_மதிப்பு(unsigned idx) மாறிலி
{
      திரும்பு vvp_scalar_t(value(idx),6,6);
}

வெறுமை vvp_wire_vec4::vec4_மதிப்பு(vvp_vector4_t&val) மாறிலி
{
      val = bits4_;
      எனில் (test_force_mask_is_zero())
	    திரும்பு;

      க்கு (unsigned idx = 0 ; idx < bits4_.size() ; idx += 1)
	    val.set_bit(idx, filtered_மதிப்பு(idx));
}

vvp_bit4_t vvp_wire_vec4::driven_மதிப்பு(unsigned idx) மாறிலி
{
      திரும்பு bits4_.value(idx);
}

தருக்க vvp_wire_vec4::is_forced(unsigned idx) மாறிலி
{
      திரும்பு test_force_mask(idx);
}

vvp_wire_vec8::vvp_wire_vec8(unsigned wid)
: bits8_(wid)
{
      needs_init_ = உண்மை;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec4(மாறிலி vvp_vector4_t&bit,
                                                 vvp_vector4_t&rep,
                                                 unsigned base,
                                                 unsigned vwid)
{
	// For now there is no support for a non-zero base.
      ப்ரதிஜ்ஞா(0 == base);
      ப்ரதிஜ்ஞா(bits8_.size() == vwid);
      ப்ரதிஜ்ஞா(bits8_.size() == bit.size());
	// QUESTION: Is it really correct to propagate a vec4 if this
	// is a vec8 node? In fact, it is really possible for a vec4
	// value to get through to a vec8 filter?
      vvp_vector8_t rep8;
      prop_t rc = filter_vec8(vvp_vector8_t(bit,6,6), rep8, 0, vwid);
      எனில் (rc == REPL)
	    rep = reduce4(rep8);

      needs_init_ = பொய்;
      திரும்பு rc;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec8(மாறிலி vvp_vector8_t&bit, vvp_vector8_t&rep, unsigned base, unsigned vwid)
{
      ப்ரதிஜ்ஞா(vwid == bits8_.size());
	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      எனில் (base==0 && bit.size()==vwid) {
	    எனில் (bits8_.eeq(bit) && !needs_init_) திரும்பு STOP;
	    bits8_ = bit;
      } இல்லையெனில் {
	    எனில் (bits8_.subvalue(base, bit.size()).eeq(bit) &&
		!needs_init_) {
		  திரும்பு STOP;
	    }
	    எனில் (bits8_.size() == 0)
		  bits8_ = vvp_vector8_t(vwid);
	    ப்ரதிஜ்ஞா(bits8_.size() == vwid);
	    bits8_.set_vec(base, bit);
      }
      needs_init_ = பொய்;
      திரும்பு filter_mask_(bit, force8_, rep, base);
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_உள்ளீடு_vec8(மாறிலி vvp_vector8_t&bit, vvp_vector8_t&rep) மாறிலி
{
      திரும்பு filter_உள்ளீடு_mask(bit, force8_, rep);
}

unsigned vvp_wire_vec8::filter_size() மாறிலி
{
      திரும்பு bits8_.size();
}

வெறுமை vvp_wire_vec8::force_fil_vec4(மாறிலி vvp_vector4_t&val, மாறிலி vvp_vector2_t&mask)
{
      force_fil_vec8(vvp_vector8_t(val,6,6), mask);
}

வெறுமை vvp_wire_vec8::force_fil_vec8(மாறிலி vvp_vector8_t&val, மாறிலி vvp_vector2_t&mask)
{
      force_mask(mask);

      எனில் (force8_.size() == 0) {
	    force8_ = val;
      } இல்லையெனில் {
	    க்கு (unsigned idx = 0; idx < mask.size() ; idx += 1) {
		  எனில் (mask.value(idx) == 0)
			தொடர்;

		  force8_.set_bit(idx, val.value(idx));
	    }
      }
      run_vpi_callbacks();
}

வெறுமை vvp_wire_vec8::force_fil_real(இரட்டை, மாறிலி vvp_vector2_t&)
{
      ப்ரதிஜ்ஞா(0);
}

வெறுமை vvp_wire_vec8::release(vvp_net_ptr_t ptr, தருக்க net_flag)
{
	// Wires revert to their unforced value after release.
      vvp_vector2_t mask (vvp_vector2_t::FILL1, bits8_.size());
      release_mask(mask);
      எனில் (net_flag) {
	    needs_init_ = !force8_ .eeq(bits8_);
	    ptr.ptr()->send_vec8(bits8_);
      } இல்லையெனில் {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    ப்ரதிஜ்ஞா(0);
//	    ptr.ptr()->fun->recv_vec8(ptr, force8_);
      }
}

வெறுமை vvp_wire_vec8::release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, தருக்க net_flag)
{
      ப்ரதிஜ்ஞா(bits8_.size() >= base + wid);

      vvp_vector2_t mask (vvp_vector2_t::FILL0, bits8_.size());
      க்கு (unsigned idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      release_mask(mask);

      எனில் (net_flag) {
	    needs_init_ = !force8_.subvalue(base,wid) .eeq((bits8_.subvalue(base,wid)));
	    ptr.ptr()->send_vec8_pv(bits8_.subvalue(base,wid),
				    base, bits8_.size());
	    run_vpi_callbacks();
      } இல்லையெனில் {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    ப்ரதிஜ்ஞா(0);
//	    ptr.ptr()->fun->recv_vec8_pv(ptr, force8_.subvalue(base,wid),
//					 base, force8_.size());
      }
}

unsigned vvp_wire_vec8::மதிப்பு_size() மாறிலி
{
      திரும்பு bits8_.size();
}

vvp_scalar_t vvp_wire_vec8::filtered_மதிப்பு(unsigned idx) மாறிலி
{
      எனில் (test_force_mask(idx))
	    திரும்பு force8_.value(idx);
      இல்லையெனில்
	    திரும்பு bits8_.value(idx);
}

vvp_bit4_t vvp_wire_vec8::value(unsigned idx) மாறிலி
{
      திரும்பு filtered_மதிப்பு(idx).value();
}

vvp_scalar_t vvp_wire_vec8::scalar_மதிப்பு(unsigned idx) மாறிலி
{
      திரும்பு filtered_மதிப்பு(idx);
}

vvp_vector8_t vvp_wire_vec8::vec8_மதிப்பு() மாறிலி
{
      vvp_vector8_t tmp = bits8_;
      க்கு (unsigned idx = 0 ; idx < bits8_.size() ; idx += 1)
	    tmp.set_bit(idx, filtered_மதிப்பு(idx));
      திரும்பு tmp;
}

வெறுமை vvp_wire_vec8::vec4_மதிப்பு(vvp_vector4_t&val) மாறிலி
{
      val = reduce4(vec8_மதிப்பு());
}

vvp_bit4_t vvp_wire_vec8::driven_மதிப்பு(unsigned idx) மாறிலி
{
      திரும்பு bits8_.value(idx).value();
}

தருக்க vvp_wire_vec8::is_forced(unsigned idx) மாறிலி
{
      திரும்பு test_force_mask(idx);
}

vvp_wire_real::vvp_wire_real()
: bit_(0.0), force_(0.0)
{
}

vvp_net_fil_t::prop_t vvp_wire_real::filter_real(இரட்டை&bit)
{
      எனில் (bits_equal(bit_, bit) && !needs_init_) திரும்பு STOP;
      bit_ = bit;
      needs_init_ = பொய்;
      திரும்பு filter_mask_(bit, force_);
}

unsigned vvp_wire_real::filter_size() மாறிலி
{
      ப்ரதிஜ்ஞா(0);
      திரும்பு 0;
}

வெறுமை vvp_wire_real::force_fil_vec4(மாறிலி vvp_vector4_t&, மாறிலி vvp_vector2_t&)
{
      ப்ரதிஜ்ஞா(0);
}

வெறுமை vvp_wire_real::force_fil_vec8(மாறிலி vvp_vector8_t&, மாறிலி vvp_vector2_t&)
{
      ப்ரதிஜ்ஞா(0);
}

வெறுமை vvp_wire_real::force_fil_real(இரட்டை val, மாறிலி vvp_vector2_t&mask)
{
      force_mask(mask);
      எனில் (mask.value(0))
	    force_ = val;

      run_vpi_callbacks();
}

வெறுமை vvp_wire_real::release(vvp_net_ptr_t ptr, தருக்க net_flag)
{
      vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
      எனில் (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_mask(mask);
	    needs_init_ = !bits_equal(force_, bit_);
	    ptr.ptr()->send_real(bit_, 0);
      } இல்லையெனில் {
	      // Variables keep the current value.
	    இரட்டை res =  real_மதிப்பு();
	    release_mask(mask);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
}

வெறுமை vvp_wire_real::release_pv(vvp_net_ptr_t, unsigned, unsigned, தருக்க)
{
      ப்ரதிஜ்ஞா(0);
#எனில் 0
	// A real is a single value. If for some reason this part release
	// can happen the following code should work correctly (requires
	// a base of 0 and a width of 1).
      vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
      ப்ரதிஜ்ஞா(base == 0 && wid == 1);

      எனில் (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_mask(mask);
	    ptr.ptr()->send_real(bit_, 0);
      } இல்லையெனில் {
	      // Variables keep the current value.
	    இரட்டை res =  real_மதிப்பு();
	    release_mask(mask);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
#endif
}

unsigned vvp_wire_real::மதிப்பு_size() மாறிலி
{
      ப்ரதிஜ்ஞா(0);
      திரும்பு 1;
}

vvp_bit4_t vvp_wire_real::value(unsigned) மாறிலி
{
      ப்ரதிஜ்ஞா(0);
      திரும்பு BIT4_X;
}

vvp_scalar_t vvp_wire_real::scalar_மதிப்பு(unsigned) மாறிலி
{
      ப்ரதிஜ்ஞா(0);
      திரும்பு vvp_scalar_t();
}

வெறுமை vvp_wire_real::vec4_மதிப்பு(vvp_vector4_t&) மாறிலி
{
      ப்ரதிஜ்ஞா(0);
}

இரட்டை vvp_wire_real::real_மதிப்பு() மாறிலி
{
      எனில் (test_force_mask(0))
	    திரும்பு force_;
      இல்லையெனில்
	    திரும்பு bit_;
}

#எனில் 0
vvp_wire_string::vvp_wire_string()
{
}

unsigned vvp_wire_string::filter_size() மாறிலி
{
      ப்ரதிஜ்ஞா(0);
      திரும்பு 0;
}

வெறுமை vvp_wire_string::force_fil_vec4(மாறிலி vvp_vector4_t&, மாறிலி vvp_vector2_t&)
{
      ப்ரதிஜ்ஞா(0);
}
வெறுமை vvp_wire_string::force_fil_vec8(மாறிலி vvp_vector8_t&, மாறிலி vvp_vector2_t&)
{
      ப்ரதிஜ்ஞா(0);
}
வெறுமை vvp_wire_string::force_fil_real(இரட்டை, மாறிலி vvp_vector2_t&)
{
      ப்ரதிஜ்ஞா(0);
}

வெறுமை vvp_wire_string::release(vvp_net_ptr_t ptr, தருக்க net_flag)
{
      ப்ரதிஜ்ஞா(0);
}

வெறுமை vvp_wire_string::release_pv(vvp_net_ptr_t, unsigned, unsigned, தருக்க)
{
      ப்ரதிஜ்ஞா(0);
}

unsigned vvp_wire_string::மதிப்பு_size() மாறிலி
{
      ப்ரதிஜ்ஞா(0);
      திரும்பு 1;
}

vvp_bit4_t vvp_wire_string::value(unsigned) மாறிலி
{
      ப்ரதிஜ்ஞா(0);
      திரும்பு BIT4_X;
}

vvp_scalar_t vvp_wire_string::scalar_மதிப்பு(unsigned) மாறிலி
{
      ப்ரதிஜ்ஞா(0);
      திரும்பு vvp_scalar_t();
}

வெறுமை vvp_wire_string::vec4_மதிப்பு(vvp_vector4_t&) மாறிலி
{
      ப்ரதிஜ்ஞா(0);
}

இரட்டை vvp_wire_string::real_மதிப்பு() மாறிலி
{
      ப்ரதிஜ்ஞா(0);
      திரும்பு 0.0;
}
#endif
