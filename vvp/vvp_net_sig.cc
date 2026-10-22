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

using നാമാകാശ std;

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
template <വർഗ്ഗം T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_mask_(സ്ഥിരം T&val, സ്ഥിരം T&force, T&filter, unsigned base)
{
      എങ്കിൽ (!test_force_mask_is_zero()) {
	      // Some bits are being forced. Go through the
	      // force_mask_ and force value to see which bits are
	      // propagated and which are kept from the forced
	      // value. Update the filter with the filtered result and
	      // return REPL to indicate that some bits have changed,
	      // or STOP if no bits change.
	    തർക്കം propagate_flag = force_propagate_;
	    force_propagate_ = അസത്യം;
	    പ്രതിജ്ഞാ(force_mask_.size() == force.size());
	    പ്രതിജ്ഞാ((base+val.size()) <= force_mask_.size());

	    filter = val;
	    വേണ്ടി (unsigned idx = 0 ; idx < val.size() ; idx += 1) {
		  എങ്കിൽ (force_mask_.value(base+idx))
			filter.set_bit(idx, force.value(base+idx));
		  അല്ലെങ്കിൽ
			propagate_flag = സത്യം;
	    }

	    എങ്കിൽ (propagate_flag) {
		  run_vpi_callbacks();
		  തിരികെ REPL;
	    } അല്ലെങ്കിൽ {
		  തിരികെ STOP;
	    }

      } അല്ലെങ്കിൽ {
	    run_vpi_callbacks();
	    തിരികെ PROP;
      }
}

template <വർഗ്ഗം T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_mask_(T&val, T force)
{

      എങ്കിൽ (test_force_mask(0)) {
	    val = force;
	    run_vpi_callbacks();
	    തിരികെ REPL;
      }
      run_vpi_callbacks();
      തിരികെ PROP;
}

template <വർഗ്ഗം T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_നിവേശം_mask(സ്ഥിരം T&val, സ്ഥിരം T&force, T&rep) സ്ഥിരം
{
      എങ്കിൽ (test_force_mask_is_zero())
	    തിരികെ PROP;

      പ്രതിജ്ഞാ(force_mask_.size() == force.size());

      rep = val;
      വേണ്ടി (unsigned idx = 0 ; idx < val.size() ; idx += 1) {
	    എങ്കിൽ (force_mask_.value(idx))
		  rep.set_bit(idx, force.value(idx));
      }

      തിരികെ REPL;
}

vvp_signal_മൂല്യം::~vvp_signal_മൂല്യം()
{
}

ഇരട്ട vvp_signal_മൂല്യം::real_മൂല്യം() സ്ഥിരം
{
      പ്രതിജ്ഞാ(0);
      തിരികെ 0;
}

ശൂന്യം vvp_net_t::force_vec4(സ്ഥിരം vvp_vector4_t&val, സ്ഥിരം vvp_vector2_t&mask)
{
      പ്രതിജ്ഞാ(fil);
      fil->force_fil_vec4(val, mask);
      fun->force_flag(അസത്യം);
      vvp_send_vec4(out_, val, 0);
}

ശൂന്യം vvp_net_t::force_vec8(സ്ഥിരം vvp_vector8_t&val, സ്ഥിരം vvp_vector2_t&mask)
{
      പ്രതിജ്ഞാ(fil);
      fil->force_fil_vec8(val, mask);
      fun->force_flag(അസത്യം);
      vvp_send_vec8(out_, val);
}

ശൂന്യം vvp_net_t::force_real(ഇരട്ട val, സ്ഥിരം vvp_vector2_t&mask)
{
      പ്രതിജ്ഞാ(fil);
      fil->force_fil_real(val, mask);
      fun->force_flag(അസത്യം);
      vvp_send_real(out_, val, 0);
}

/* **** vvp_fun_signal methods **** */

vvp_fun_signal_base::vvp_fun_signal_base()
{
      continuous_assign_active_ = അസത്യം;
      needs_init_ = സത്യം;
      cassign_link = 0;
      എണ്ണം_functors_sig += 1;
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
ശൂന്യം vvp_fun_signal4_sa::recv_vec4(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector4_t&bit,
                                   vvp_context_t)
{
      തിരഞ്ഞെടുപ്പ് (ptr.port()) {
	  അവസ്ഥ 0: // Normal input (feed from net, or set from process)
	      /* If we don't have a continuous assign mask then just
		 copy the bits, otherwise we need to see if there are
		 any holes in the mask so we can set those bits. */
	    എങ്കിൽ (assign_mask_.size() == 0) {
                  എങ്കിൽ (needs_init_ || !bits4_.eeq(bit)) {
			പ്രതിജ്ഞാ(bit.size() == bits4_.size());
			bits4_ = bit;
			needs_init_ = അസത്യം;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    } അല്ലെങ്കിൽ {
		  തർക്കം changed = അസത്യം;
		  പ്രതിജ്ഞാ(bits4_.size() == assign_mask_.size());
		  വേണ്ടി (unsigned idx = 0 ;  idx < bit.size() ;  idx += 1) {
			എങ്കിൽ (idx >= bits4_.size()) നിർത്തുക;
			എങ്കിൽ (assign_mask_.value(idx)) തുടരുക;
			bits4_.set_bit(idx, bit.value(idx));
			changed = സത്യം;
		  }
		  എങ്കിൽ (changed) {
			needs_init_ = അസത്യം;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    }
	    നിർത്തുക;

	  അവസ്ഥ 1: // Continuous assign value
	      // Handle the simple case of the linked source being wider
	      // than this signal. Note we don't yet support the case of
	      // the linked source being narrower than this signal, or
	      // the case of an expression being assigned.
	    bits4_ = coerce_to_width(bit, bits4_.size());
	    assign_mask_ = vvp_vector2_t(vvp_vector2_t::FILL1, bits4_.size());
	    ptr.ptr()->send_vec4(bits4_, 0);
	    നിർത്തുക;

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    പ്രതിജ്ഞാ(0);
	    നിർത്തുക;
      }
}

ശൂന്യം vvp_fun_signal4_sa::recv_vec8(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector8_t&bit)
{
      recv_vec4(ptr, reduce4(bit), 0);
}

ശൂന്യം vvp_fun_signal4_sa::recv_vec4_pv(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector4_t&bit,
				      unsigned base, unsigned vwid, vvp_context_t)
{
      പ്രതിജ്ഞാ(bits4_.size() == vwid);
      unsigned wid = bit.size();

      തിരഞ്ഞെടുപ്പ് (ptr.port()) {
	  അവസ്ഥ 0: // Normal input
	    എങ്കിൽ (assign_mask_.size() == 0) {
                  വേണ്ടി (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
			എങ്കിൽ (base+idx >= bits4_.size()) നിർത്തുക;
			bits4_.set_bit(base+idx, bit.value(idx));
		  }
		  needs_init_ = അസത്യം;
		  ptr.ptr()->send_vec4(bits4_,0);
	    } അല്ലെങ്കിൽ {
		  തർക്കം changed = അസത്യം;
		  പ്രതിജ്ഞാ(bits4_.size() == assign_mask_.size());
		  വേണ്ടി (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
			എങ്കിൽ (base+idx >= bits4_.size()) നിർത്തുക;
			എങ്കിൽ (assign_mask_.value(base+idx)) തുടരുക;
			bits4_.set_bit(base+idx, bit.value(idx));
			changed = സത്യം;
		  }
		  എങ്കിൽ (changed) {
			needs_init_ = അസത്യം;
			ptr.ptr()->send_vec4(bits4_,0);
		  }
	    }
	    നിർത്തുക;

	  അവസ്ഥ 1: // Continuous assign value
	    എങ്കിൽ (assign_mask_.size() == 0)
		  assign_mask_ = vvp_vector2_t(vvp_vector2_t::FILL0, bits4_.size());
	    വേണ്ടി (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  എങ്കിൽ (base+idx >= bits4_.size())
			നിർത്തുക;
		  bits4_.set_bit(base+idx, bit.value(idx));
		  assign_mask_.set_bit(base+idx, 1);
	    }
	    ptr.ptr()->send_vec4(bits4_,0);
	    നിർത്തുക;

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    പ്രതിജ്ഞാ(0);
	    നിർത്തുക;
      }
}

ശൂന്യം vvp_fun_signal4_sa::recv_vec8_pv(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector8_t&bit,
				      unsigned base, unsigned vwid)
{
      recv_vec4_pv(ptr, reduce4(bit), base, vwid, 0);
}

ശൂന്യം vvp_fun_signal_base::deassign()
{
      continuous_assign_active_ = അസത്യം;
      assign_mask_ = vvp_vector2_t();
}

ശൂന്യം vvp_fun_signal_base::deassign_pv(unsigned base, unsigned wid)
{
      വേണ്ടി (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    assign_mask_.set_bit(base+idx, 0);
      }

      എങ്കിൽ (assign_mask_.is_zero()) {
	    assign_mask_ = vvp_vector2_t();
      }
}

ശൂന്യം automatic_signal_base::release(vvp_net_ptr_t,തർക്കം)
{
      പ്രതിജ്ഞാ(0);
}

ശൂന്യം automatic_signal_base::release_pv(vvp_net_ptr_t,unsigned,unsigned,തർക്കം)
{
      പ്രതിജ്ഞാ(0);
}

unsigned automatic_signal_base::filter_size() സ്ഥിരം
{
      പ്രതിജ്ഞാ(0);
      തിരികെ(0);
}
ശൂന്യം automatic_signal_base::force_fil_vec4(സ്ഥിരം vvp_vector4_t&, സ്ഥിരം vvp_vector2_t&)
{
      പ്രതിജ്ഞാ(0);
}
ശൂന്യം automatic_signal_base::force_fil_vec8(സ്ഥിരം vvp_vector8_t&, സ്ഥിരം vvp_vector2_t&)
{
      പ്രതിജ്ഞാ(0);
}
ശൂന്യം automatic_signal_base::force_fil_real(ഇരട്ട, സ്ഥിരം vvp_vector2_t&)
{
      പ്രതിജ്ഞാ(0);
}
ശൂന്യം automatic_signal_base::get_മൂല്യം(ഘടന t_vpi_മൂല്യം*)
{
      പ്രതിജ്ഞാ(0);
}

സ്ഥിരം vvp_vector4_t& vvp_fun_signal4_sa::vec4_unfiltered_മൂല്യം() സ്ഥിരം
{
      തിരികെ bits4_;
}

vvp_fun_signal4_aa::vvp_fun_signal4_aa(unsigned wid, vvp_bit4_t init)
{
      context_idx_ = vpip_add_item_to_context(ഇത്, vpip_peek_context_scope());
      size_ = wid;
      init_ = init;
}

vvp_fun_signal4_aa::~vvp_fun_signal4_aa()
{
      പ്രതിജ്ഞാ(0);
}

ശൂന്യം vvp_fun_signal4_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, പുതിയ vvp_vector4_t(size_, init_));
}

ശൂന്യം vvp_fun_signal4_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*bits = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      bits->fill_bits(init_);
}

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം vvp_fun_signal4_aa::free_instance(vvp_context_t context)
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
ശൂന്യം vvp_fun_signal4_aa::recv_vec4(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      പ്രതിജ്ഞാ(ptr.port() == 0);
      പ്രതിജ്ഞാ(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      എങ്കിൽ (!bits4->eeq(bit)) {
            *bits4 = bit;
            ptr.ptr()->send_vec4(*bits4, context);
      }
}

ശൂന്യം vvp_fun_signal4_aa::recv_vec4_pv(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector4_t&bit,
				      unsigned base, unsigned vwid, vvp_context_t context)
{
      പ്രതിജ്ഞാ(ptr.port() == 0);
      പ്രതിജ്ഞാ(size_ == vwid);
      പ്രതിജ്ഞാ(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      unsigned wid = bit.size();
      വേണ്ടി (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
            എങ്കിൽ (base+idx >= bits4->size()) നിർത്തുക;
            bits4->set_bit(base+idx, bit.value(idx));
      }
      ptr.ptr()->send_vec4(*bits4, context);
}

unsigned vvp_fun_signal4_aa::മൂല്യം_size() സ്ഥിരം
{
      തിരികെ size_;
}

vvp_bit4_t vvp_fun_signal4_aa::value(unsigned idx) സ്ഥിരം
{
      സ്ഥിരം vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      തിരികെ bits4->value(idx);
}

vvp_scalar_t vvp_fun_signal4_aa::scalar_മൂല്യം(unsigned idx) സ്ഥിരം
{
      സ്ഥിരം vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      തിരികെ vvp_scalar_t(bits4->value(idx), 6, 6);
}

ശൂന്യം vvp_fun_signal4_aa::vec4_മൂല്യം(vvp_vector4_t&val) സ്ഥിരം
{
      സ്ഥിരം vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      val = *bits4;
}

സ്ഥിരം vvp_vector4_t&vvp_fun_signal4_aa::vec4_unfiltered_മൂല്യം() സ്ഥിരം
{
      സ്ഥിരം vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      തിരികെ *bits4;
}

ശൂന്യം vvp_fun_signal4_aa::operator delete(ശൂന്യം*)
{
      പ്രതിജ്ഞാ(0);
}

/*
 * Testing for equality, we want a bitwise test instead of an
 * arithmetic test because we want to treat for example -0 different
 * from +0.
 */
തർക്കം bits_equal(ഇരട്ട a, ഇരട്ട b)
{
      തിരികെ memcmp(&a, &b, വലുപ്പം a) == 0;
}

vvp_fun_signal_real_sa::vvp_fun_signal_real_sa()
{
      bits_ = 0.0;
}

ഇരട്ട vvp_fun_signal_real_sa::real_unfiltered_മൂല്യം() സ്ഥിരം
{
      തിരികെ bits_;
}

ശൂന്യം vvp_fun_signal_real_sa::recv_real(vvp_net_ptr_t ptr, ഇരട്ട bit,
                                       vvp_context_t)
{
      തിരഞ്ഞെടുപ്പ് (ptr.port()) {
	  അവസ്ഥ 0:
	    എങ്കിൽ (!continuous_assign_active_) {
                  എങ്കിൽ (needs_init_ || !bits_equal(bits_, bit)) {
			bits_ = bit;
			needs_init_ = അസത്യം;
			ptr.ptr()->send_real(bit, 0);
		  }
	    }
	    നിർത്തുക;

	  അവസ്ഥ 1: // Continuous assign value
	    continuous_assign_active_ = സത്യം;
	    bits_ = bit;
	    ptr.ptr()->send_real(bit, 0);
	    നിർത്തുക;

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    പ്രതിജ്ഞാ(0);
	    നിർത്തുക;
      }
}

vvp_fun_signal_real_aa::vvp_fun_signal_real_aa()
{
      context_idx_ = vpip_add_item_to_context(ഇത്, vpip_peek_context_scope());
}

vvp_fun_signal_real_aa::~vvp_fun_signal_real_aa()
{
      പ്രതിജ്ഞാ(0);
}

ശൂന്യം vvp_fun_signal_real_aa::alloc_instance(vvp_context_t context)
{
      ഇരട്ട*bits = പുതിയ ഇരട്ട;
      vvp_set_context_item(context, context_idx_, bits);

      *bits = 0.0;
}

ശൂന്യം vvp_fun_signal_real_aa::reset_instance(vvp_context_t context)
{
      ഇരട്ട*bits = static_cast<ഇരട്ട*>
            (vvp_get_context_item(context, context_idx_));

      *bits = 0.0;
}

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം vvp_fun_signal_real_aa::free_instance(vvp_context_t context)
{
      ഇരട്ട*bits = static_cast<ഇരട്ട*>
            (vvp_get_context_item(context, context_idx_));
      delete bits;
}
#endif

ഇരട്ട vvp_fun_signal_real_aa::real_unfiltered_മൂല്യം() സ്ഥിരം
{
      സ്ഥിരം ഇരട്ട*bits = static_cast<ഇരട്ട*>
            (vthread_get_rd_context_item(context_idx_));

      തിരികെ *bits;
}

ഇരട്ട vvp_fun_signal_real_aa::real_മൂല്യം() സ്ഥിരം
{
      തിരികെ real_unfiltered_മൂല്യം();
}

ശൂന്യം vvp_fun_signal_real_aa::recv_real(vvp_net_ptr_t ptr, ഇരട്ട bit,
                                       vvp_context_t context)
{
      പ്രതിജ്ഞാ(ptr.port() == 0);
      പ്രതിജ്ഞാ(context);

      ഇരട്ട*bits = static_cast<ഇരട്ട*>
            (vvp_get_context_item(context, context_idx_));

      എങ്കിൽ (!bits_equal(*bits,bit)) {
            *bits = bit;
            ptr.ptr()->send_real(bit, context);
      }
}

unsigned vvp_fun_signal_real_aa::മൂല്യം_size() സ്ഥിരം
{
      പ്രതിജ്ഞാ(0);
      തിരികെ 1;
}

vvp_bit4_t vvp_fun_signal_real_aa::value(unsigned) സ്ഥിരം
{
      പ്രതിജ്ഞാ(0);
      തിരികെ BIT4_X;
}

vvp_scalar_t vvp_fun_signal_real_aa::scalar_മൂല്യം(unsigned) സ്ഥിരം
{
      പ്രതിജ്ഞാ(0);
      തിരികെ vvp_scalar_t();
}

ശൂന്യം vvp_fun_signal_real_aa::vec4_മൂല്യം(vvp_vector4_t&) സ്ഥിരം
{
      പ്രതിജ്ഞാ(0);
}

ശൂന്യം* vvp_fun_signal_real_aa::operator പുതിയ(std::size_t size)
{
      തിരികെ vvp_net_fun_t::heap_.alloc(size);
}

ശൂന്യം vvp_fun_signal_real_aa::operator delete(ശൂന്യം*)
{
      പ്രതിജ്ഞാ(0);
}


vvp_fun_signal_string_sa::vvp_fun_signal_string_sa()
{
}

ശൂന്യം vvp_fun_signal_string_sa::recv_string(vvp_net_ptr_t ptr, സ്ഥിരം std::string&bit,
					   vvp_context_t)
{
      പ്രതിജ്ഞാ(ptr.port() == 0);

      എങ്കിൽ (needs_init_ || മൂല്യം_2 != bit) {
	    മൂല്യം_2 = bit;
	    needs_init_ = അസത്യം;

	    ptr.ptr()->send_string(bit, 0);
      }
}

സ്ഥിരം string& vvp_fun_signal_string_sa::get_string() സ്ഥിരം
{
      തിരികെ മൂല്യം_2;
}

vvp_fun_signal_string_aa::vvp_fun_signal_string_aa()
{
      context_idx_ = vpip_add_item_to_context(ഇത്, vpip_peek_context_scope());
}

vvp_fun_signal_string_aa::~vvp_fun_signal_string_aa()
{
      പ്രതിജ്ഞാ(0);
}

ശൂന്യം vvp_fun_signal_string_aa::alloc_instance(vvp_context_t context)
{
      string*bits = പുതിയ std::string;
      vvp_set_context_item(context, context_idx_, bits);
      *bits = "";
}

ശൂന്യം vvp_fun_signal_string_aa::reset_instance(vvp_context_t context)
{
      string*bits = static_cast<std::string*>
	    (vvp_get_context_item(context, context_idx_));
      *bits = "";
}

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം vvp_fun_signal_string_aa::free_instance(vvp_context_t context)
{
      string*bits = static_cast<std::string*>
            (vvp_get_context_item(context, context_idx_));
      delete bits;
}
#endif

ശൂന്യം vvp_fun_signal_string_aa::recv_string(vvp_net_ptr_t ptr, സ്ഥിരം std::string&bit, vvp_context_t context)
{
      പ്രതിജ്ഞാ(ptr.port() == 0);
      പ്രതിജ്ഞാ(context);

      string*bits = static_cast<std::string*>
	    (vvp_get_context_item(context, context_idx_));

      എങ്കിൽ (*bits != bit) {
	    *bits = bit;
	    ptr.ptr()->send_string(bit, context);
      }
}

unsigned vvp_fun_signal_string_aa::മൂല്യം_size() സ്ഥിരം
{
      പ്രതിജ്ഞാ(0);
      തിരികെ 1;
}

vvp_bit4_t vvp_fun_signal_string_aa::value(unsigned) സ്ഥിരം
{
      പ്രതിജ്ഞാ(0);
      തിരികെ BIT4_X;
}

vvp_scalar_t vvp_fun_signal_string_aa::scalar_മൂല്യം(unsigned) സ്ഥിരം
{
      പ്രതിജ്ഞാ(0);
      തിരികെ vvp_scalar_t();
}

ശൂന്യം vvp_fun_signal_string_aa::vec4_മൂല്യം(vvp_vector4_t&) സ്ഥിരം
{
      പ്രതിജ്ഞാ(0);
}

ഇരട്ട vvp_fun_signal_string_aa::real_മൂല്യം() സ്ഥിരം
{
      പ്രതിജ്ഞാ(0);
      തിരികെ 0.0;
}

സ്ഥിരം std::string& vvp_fun_signal_string_aa::get_string() സ്ഥിരം
{
      സ്ഥിരം string*bits = static_cast<std::string*>
            (vthread_get_rd_context_item(context_idx_));

      തിരികെ *bits;
}

ശൂന്യം* vvp_fun_signal_string_aa::operator പുതിയ(std::size_t size)
{
      തിരികെ vvp_net_fun_t::heap_.alloc(size);
}

ശൂന്യം vvp_fun_signal_string_aa::operator delete(ശൂന്യം*)
{
      പ്രതിജ്ഞാ(0);
}

  /* OBJECT signals */

vvp_fun_signal_object_sa::vvp_fun_signal_object_sa(unsigned size)
: vvp_fun_signal_object(size)
{
}

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം vvp_fun_signal_object_aa::free_instance(vvp_context_t context)
{
      vvp_object_t*bits = static_cast<vvp_object_t*>
            (vvp_get_context_item(context, context_idx_));
      delete bits;
}
#endif

ശൂന്യം vvp_fun_signal_object_sa::recv_object(vvp_net_ptr_t ptr, vvp_object_t bit,
					   vvp_context_t)
{
      പ്രതിജ്ഞാ(ptr.port() == 0);

      എങ്കിൽ (needs_init_ || മൂല്യം_2 != bit) {
	    മൂല്യം_2 = bit;
	    needs_init_ = അസത്യം;

	    ptr.ptr()->send_object(bit, 0);
      }
}

vvp_object_t vvp_fun_signal_object_sa::get_object() സ്ഥിരം
{
      തിരികെ മൂല്യം_2;
}

vvp_fun_signal_object_aa::vvp_fun_signal_object_aa(unsigned size)
: vvp_fun_signal_object(size)
{
      context_idx_ = vpip_add_item_to_context(ഇത്, vpip_peek_context_scope());
}

vvp_fun_signal_object_aa::~vvp_fun_signal_object_aa()
{
      പ്രതിജ്ഞാ(0);
}

ശൂന്യം vvp_fun_signal_object_aa::alloc_instance(vvp_context_t context)
{
      vvp_object_t*bits = പുതിയ vvp_object_t;
      vvp_set_context_item(context, context_idx_, bits);
      bits->reset();
}

ശൂന്യം vvp_fun_signal_object_aa::reset_instance(vvp_context_t context)
{
      vvp_object_t*bits = static_cast<vvp_object_t*>
	    (vvp_get_context_item(context, context_idx_));
      bits->reset();
}

vvp_object_t vvp_fun_signal_object_aa::get_object() സ്ഥിരം
{
      സ്ഥിരം vvp_object_t*bits = static_cast<vvp_object_t*>
	    (vthread_get_rd_context_item(context_idx_));
      തിരികെ *bits;
}

ശൂന്യം vvp_fun_signal_object_aa::recv_object(vvp_net_ptr_t ptr, vvp_object_t bit,
					   vvp_context_t context)
{
      പ്രതിജ്ഞാ(ptr.port() == 0);
      പ്രതിജ്ഞാ(context);

      vvp_object_t*bits = static_cast<vvp_object_t*>
	    (vvp_get_context_item(context, context_idx_));

      എങ്കിൽ (*bits != bit) {
	    *bits = bit;
	    ptr.ptr()->send_object(bit, context);
      }
}

unsigned vvp_fun_signal_object_aa::മൂല്യം_size() സ്ഥിരം
{
      തിരികെ 1;
}

vvp_bit4_t vvp_fun_signal_object_aa::value(unsigned) സ്ഥിരം
{
      പ്രതിജ്ഞാ(0);
      തിരികെ BIT4_X;
}

vvp_scalar_t vvp_fun_signal_object_aa::scalar_മൂല്യം(unsigned) സ്ഥിരം
{
      പ്രതിജ്ഞാ(0);
      തിരികെ vvp_scalar_t();
}

ശൂന്യം vvp_fun_signal_object_aa::vec4_മൂല്യം(vvp_vector4_t&) സ്ഥിരം
{
      പ്രതിജ്ഞാ(0);
}

ശൂന്യം* vvp_fun_signal_object_aa::operator പുതിയ(std::size_t size)
{
      തിരികെ vvp_net_fun_t::heap_.alloc(size);
}

ശൂന്യം vvp_fun_signal_object_aa::operator delete(ശൂന്യം*)
{
      പ്രതിജ്ഞാ(0);
}

  /* **** */

vvp_fun_force::vvp_fun_force()
{
}

vvp_fun_force::~vvp_fun_force()
{
}

ശൂന്യം vvp_fun_force::recv_vec4(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector4_t&bit,
			      vvp_context_t)
{
      പ്രതിജ്ഞാ(ptr.port() == 0);
      vvp_net_t*net = ptr.ptr();

      vvp_net_t*dst = net->port[3].ptr();
      പ്രതിജ്ഞാ(dst->fil);

      dst->force_vec4(coerce_to_width(bit, dst->fil->filter_size()), vvp_vector2_t(vvp_vector2_t::FILL1, dst->fil->filter_size()));
}

ശൂന്യം vvp_fun_force::recv_real(vvp_net_ptr_t ptr, ഇരട്ട bit, vvp_context_t)
{
      പ്രതിജ്ഞാ(ptr.port() == 0);
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

vvp_bit4_t vvp_wire_base::driven_മൂല്യം(unsigned) സ്ഥിരം
{
      പ്രതിജ്ഞാ(0);
      തിരികെ BIT4_X;
}

തർക്കം vvp_wire_base::is_forced(unsigned) സ്ഥിരം
{
      പ്രതിജ്ഞാ(0);
      തിരികെ അസത്യം;
}

vvp_wire_vec4::vvp_wire_vec4(unsigned wid, vvp_bit4_t init)
: bits4_(wid, init)
{
      needs_init_ = സത്യം;
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec4(സ്ഥിരം vvp_vector4_t&bit, vvp_vector4_t&rep,
						 unsigned base, unsigned vwid)
{
	// Special case! the input bit is 0 wid. Interpret this as a
	// vector of BIT4_X to match the width of the bits4_ vector.
	// FIXME! This is a hack to work around some buggy gate
	// implementations! This should be removed!
      എങ്കിൽ (base==0 && vwid==0) {
	    vvp_vector4_t tmp (bits4_.size(), BIT4_X);
	    എങ്കിൽ (bits4_ .eeq(tmp) && !needs_init_) തിരികെ STOP;
	    bits4_ = tmp;
	    needs_init_ = അസത്യം;
	    തിരികെ filter_mask_(tmp, force4_, rep, 0);
      }

      എങ്കിൽ (vwid != bits4_.size()) {
	    cerr << "Internal error: Input vector expected width="
		 << bits4_.size() << ", got "
		 << "bit=" << bit << ", base=" << base << ", vwid=" << vwid
		 << endl;
      }
      പ്രതിജ്ഞാ(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      എങ്കിൽ (base==0 && bit.size()==vwid) {
	    എങ്കിൽ (bits4_ .eeq( bit ) && !needs_init_) തിരികെ STOP;
	    bits4_ = bit;
      } അല്ലെങ്കിൽ {
	    തർക്കം rc = bits4_.set_vec(base, bit);
	    എങ്കിൽ (rc == അസത്യം && !needs_init_) തിരികെ STOP;
      }

      needs_init_ = അസത്യം;
      തിരികെ filter_mask_(bit, force4_, rep, base);
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec8(സ്ഥിരം vvp_vector8_t&bit,
                                                 vvp_vector8_t&rep,
                                                 unsigned base,
                                                 unsigned vwid)
{
      പ്രതിജ്ഞാ(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      vvp_vector4_t bit4 (reduce4(bit));
      എങ്കിൽ (base==0 && bit4.size()==vwid) {
	    എങ്കിൽ (bits4_ .eeq( bit4 ) && !needs_init_) തിരികെ STOP;
	    bits4_ = bit4;
      } അല്ലെങ്കിൽ {
	    തർക്കം rc = bits4_.set_vec(base, bit4);
	    എങ്കിൽ (rc == അസത്യം && !needs_init_) തിരികെ STOP;
      }

      needs_init_ = അസത്യം;
      തിരികെ filter_mask_(bit, vvp_vector8_t(force4_,6,6), rep, base);
}

unsigned vvp_wire_vec4::filter_size() സ്ഥിരം
{
      തിരികെ bits4_.size();
}

ശൂന്യം vvp_wire_vec4::force_fil_vec4(സ്ഥിരം vvp_vector4_t&val, സ്ഥിരം vvp_vector2_t&mask)
{
      force_mask(mask);

      എങ്കിൽ (force4_.size() == 0) {
	    force4_ = val;
      } അല്ലെങ്കിൽ {
	    വേണ്ടി (unsigned idx = 0; idx < mask.size() ; idx += 1) {
		  എങ്കിൽ (mask.value(idx) == 0)
			തുടരുക;

		  force4_.set_bit(idx, val.value(idx));
	    }
      }
      run_vpi_callbacks();
}

ശൂന്യം vvp_wire_vec4::force_fil_vec8(സ്ഥിരം vvp_vector8_t&, സ്ഥിരം vvp_vector2_t&)
{
      പ്രതിജ്ഞാ(0);
}

ശൂന്യം vvp_wire_vec4::force_fil_real(ഇരട്ട, സ്ഥിരം vvp_vector2_t&)
{
      പ്രതിജ്ഞാ(0);
}

ശൂന്യം vvp_wire_vec4::release(vvp_net_ptr_t ptr, തർക്കം net_flag)
{
      vvp_vector2_t mask (vvp_vector2_t::FILL1, bits4_.size());
      എങ്കിൽ (net_flag) {
	      // Wires revert to their unforced value after release.
            release_mask(mask);
	    needs_init_ = ! force4_ .eeq(bits4_);
	    ptr.ptr()->send_vec4(bits4_, 0);
	    run_vpi_callbacks();
      } അല്ലെങ്കിൽ {
	      // Variables keep the current value.
	    vvp_vector4_t res (bits4_.size());
	    വേണ്ടി (unsigned idx=0; idx<bits4_.size(); idx += 1)
		  res.set_bit(idx,value(idx));
            release_mask(mask);
	    ptr.ptr()->fun->recv_vec4(ptr, res, 0);
      }
}

ശൂന്യം vvp_wire_vec4::release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, തർക്കം net_flag)
{
      പ്രതിജ്ഞാ(bits4_.size() >= base + wid);

      vvp_vector2_t mask (vvp_vector2_t::FILL0, bits4_.size());
      വേണ്ടി (unsigned idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      എങ്കിൽ (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_mask(mask);
	    needs_init_ = ! force4_.subvalue(base,wid) .eeq(bits4_.subvalue(base,wid));
	    ptr.ptr()->send_vec4_pv(bits4_.subvalue(base,wid),
				    base, bits4_.size(), 0);
	    run_vpi_callbacks();
      } അല്ലെങ്കിൽ {
	      // Variables keep the current value.
	    vvp_vector4_t res (wid);
	    വേണ്ടി (unsigned idx=0; idx<wid; idx += 1)
		  res.set_bit(idx,value(base+idx));
	    release_mask(mask);
	    ptr.ptr()->fun->recv_vec4_pv(ptr, res, base, bits4_.size(), 0);
      }
}

unsigned vvp_wire_vec4::മൂല്യം_size() സ്ഥിരം
{
      തിരികെ bits4_.size();
}

vvp_bit4_t vvp_wire_vec4::filtered_മൂല്യം(unsigned idx) സ്ഥിരം
{
      എങ്കിൽ (test_force_mask(idx))
	    തിരികെ force4_.value(idx);
      അല്ലെങ്കിൽ
	    തിരികെ bits4_.value(idx);
}

vvp_bit4_t vvp_wire_vec4::value(unsigned idx) സ്ഥിരം
{
      തിരികെ filtered_മൂല്യം(idx);
}

vvp_scalar_t vvp_wire_vec4::scalar_മൂല്യം(unsigned idx) സ്ഥിരം
{
      തിരികെ vvp_scalar_t(value(idx),6,6);
}

ശൂന്യം vvp_wire_vec4::vec4_മൂല്യം(vvp_vector4_t&val) സ്ഥിരം
{
      val = bits4_;
      എങ്കിൽ (test_force_mask_is_zero())
	    തിരികെ;

      വേണ്ടി (unsigned idx = 0 ; idx < bits4_.size() ; idx += 1)
	    val.set_bit(idx, filtered_മൂല്യം(idx));
}

vvp_bit4_t vvp_wire_vec4::driven_മൂല്യം(unsigned idx) സ്ഥിരം
{
      തിരികെ bits4_.value(idx);
}

തർക്കം vvp_wire_vec4::is_forced(unsigned idx) സ്ഥിരം
{
      തിരികെ test_force_mask(idx);
}

vvp_wire_vec8::vvp_wire_vec8(unsigned wid)
: bits8_(wid)
{
      needs_init_ = സത്യം;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec4(സ്ഥിരം vvp_vector4_t&bit,
                                                 vvp_vector4_t&rep,
                                                 unsigned base,
                                                 unsigned vwid)
{
	// For now there is no support for a non-zero base.
      പ്രതിജ്ഞാ(0 == base);
      പ്രതിജ്ഞാ(bits8_.size() == vwid);
      പ്രതിജ്ഞാ(bits8_.size() == bit.size());
	// QUESTION: Is it really correct to propagate a vec4 if this
	// is a vec8 node? In fact, it is really possible for a vec4
	// value to get through to a vec8 filter?
      vvp_vector8_t rep8;
      prop_t rc = filter_vec8(vvp_vector8_t(bit,6,6), rep8, 0, vwid);
      എങ്കിൽ (rc == REPL)
	    rep = reduce4(rep8);

      needs_init_ = അസത്യം;
      തിരികെ rc;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec8(സ്ഥിരം vvp_vector8_t&bit, vvp_vector8_t&rep, unsigned base, unsigned vwid)
{
      പ്രതിജ്ഞാ(vwid == bits8_.size());
	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      എങ്കിൽ (base==0 && bit.size()==vwid) {
	    എങ്കിൽ (bits8_.eeq(bit) && !needs_init_) തിരികെ STOP;
	    bits8_ = bit;
      } അല്ലെങ്കിൽ {
	    എങ്കിൽ (bits8_.subvalue(base, bit.size()).eeq(bit) &&
		!needs_init_) {
		  തിരികെ STOP;
	    }
	    എങ്കിൽ (bits8_.size() == 0)
		  bits8_ = vvp_vector8_t(vwid);
	    പ്രതിജ്ഞാ(bits8_.size() == vwid);
	    bits8_.set_vec(base, bit);
      }
      needs_init_ = അസത്യം;
      തിരികെ filter_mask_(bit, force8_, rep, base);
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_നിവേശം_vec8(സ്ഥിരം vvp_vector8_t&bit, vvp_vector8_t&rep) സ്ഥിരം
{
      തിരികെ filter_നിവേശം_mask(bit, force8_, rep);
}

unsigned vvp_wire_vec8::filter_size() സ്ഥിരം
{
      തിരികെ bits8_.size();
}

ശൂന്യം vvp_wire_vec8::force_fil_vec4(സ്ഥിരം vvp_vector4_t&val, സ്ഥിരം vvp_vector2_t&mask)
{
      force_fil_vec8(vvp_vector8_t(val,6,6), mask);
}

ശൂന്യം vvp_wire_vec8::force_fil_vec8(സ്ഥിരം vvp_vector8_t&val, സ്ഥിരം vvp_vector2_t&mask)
{
      force_mask(mask);

      എങ്കിൽ (force8_.size() == 0) {
	    force8_ = val;
      } അല്ലെങ്കിൽ {
	    വേണ്ടി (unsigned idx = 0; idx < mask.size() ; idx += 1) {
		  എങ്കിൽ (mask.value(idx) == 0)
			തുടരുക;

		  force8_.set_bit(idx, val.value(idx));
	    }
      }
      run_vpi_callbacks();
}

ശൂന്യം vvp_wire_vec8::force_fil_real(ഇരട്ട, സ്ഥിരം vvp_vector2_t&)
{
      പ്രതിജ്ഞാ(0);
}

ശൂന്യം vvp_wire_vec8::release(vvp_net_ptr_t ptr, തർക്കം net_flag)
{
	// Wires revert to their unforced value after release.
      vvp_vector2_t mask (vvp_vector2_t::FILL1, bits8_.size());
      release_mask(mask);
      എങ്കിൽ (net_flag) {
	    needs_init_ = !force8_ .eeq(bits8_);
	    ptr.ptr()->send_vec8(bits8_);
      } അല്ലെങ്കിൽ {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    പ്രതിജ്ഞാ(0);
//	    ptr.ptr()->fun->recv_vec8(ptr, force8_);
      }
}

ശൂന്യം vvp_wire_vec8::release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, തർക്കം net_flag)
{
      പ്രതിജ്ഞാ(bits8_.size() >= base + wid);

      vvp_vector2_t mask (vvp_vector2_t::FILL0, bits8_.size());
      വേണ്ടി (unsigned idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      release_mask(mask);

      എങ്കിൽ (net_flag) {
	    needs_init_ = !force8_.subvalue(base,wid) .eeq((bits8_.subvalue(base,wid)));
	    ptr.ptr()->send_vec8_pv(bits8_.subvalue(base,wid),
				    base, bits8_.size());
	    run_vpi_callbacks();
      } അല്ലെങ്കിൽ {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    പ്രതിജ്ഞാ(0);
//	    ptr.ptr()->fun->recv_vec8_pv(ptr, force8_.subvalue(base,wid),
//					 base, force8_.size());
      }
}

unsigned vvp_wire_vec8::മൂല്യം_size() സ്ഥിരം
{
      തിരികെ bits8_.size();
}

vvp_scalar_t vvp_wire_vec8::filtered_മൂല്യം(unsigned idx) സ്ഥിരം
{
      എങ്കിൽ (test_force_mask(idx))
	    തിരികെ force8_.value(idx);
      അല്ലെങ്കിൽ
	    തിരികെ bits8_.value(idx);
}

vvp_bit4_t vvp_wire_vec8::value(unsigned idx) സ്ഥിരം
{
      തിരികെ filtered_മൂല്യം(idx).value();
}

vvp_scalar_t vvp_wire_vec8::scalar_മൂല്യം(unsigned idx) സ്ഥിരം
{
      തിരികെ filtered_മൂല്യം(idx);
}

vvp_vector8_t vvp_wire_vec8::vec8_മൂല്യം() സ്ഥിരം
{
      vvp_vector8_t tmp = bits8_;
      വേണ്ടി (unsigned idx = 0 ; idx < bits8_.size() ; idx += 1)
	    tmp.set_bit(idx, filtered_മൂല്യം(idx));
      തിരികെ tmp;
}

ശൂന്യം vvp_wire_vec8::vec4_മൂല്യം(vvp_vector4_t&val) സ്ഥിരം
{
      val = reduce4(vec8_മൂല്യം());
}

vvp_bit4_t vvp_wire_vec8::driven_മൂല്യം(unsigned idx) സ്ഥിരം
{
      തിരികെ bits8_.value(idx).value();
}

തർക്കം vvp_wire_vec8::is_forced(unsigned idx) സ്ഥിരം
{
      തിരികെ test_force_mask(idx);
}

vvp_wire_real::vvp_wire_real()
: bit_(0.0), force_(0.0)
{
}

vvp_net_fil_t::prop_t vvp_wire_real::filter_real(ഇരട്ട&bit)
{
      എങ്കിൽ (bits_equal(bit_, bit) && !needs_init_) തിരികെ STOP;
      bit_ = bit;
      needs_init_ = അസത്യം;
      തിരികെ filter_mask_(bit, force_);
}

unsigned vvp_wire_real::filter_size() സ്ഥിരം
{
      പ്രതിജ്ഞാ(0);
      തിരികെ 0;
}

ശൂന്യം vvp_wire_real::force_fil_vec4(സ്ഥിരം vvp_vector4_t&, സ്ഥിരം vvp_vector2_t&)
{
      പ്രതിജ്ഞാ(0);
}

ശൂന്യം vvp_wire_real::force_fil_vec8(സ്ഥിരം vvp_vector8_t&, സ്ഥിരം vvp_vector2_t&)
{
      പ്രതിജ്ഞാ(0);
}

ശൂന്യം vvp_wire_real::force_fil_real(ഇരട്ട val, സ്ഥിരം vvp_vector2_t&mask)
{
      force_mask(mask);
      എങ്കിൽ (mask.value(0))
	    force_ = val;

      run_vpi_callbacks();
}

ശൂന്യം vvp_wire_real::release(vvp_net_ptr_t ptr, തർക്കം net_flag)
{
      vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
      എങ്കിൽ (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_mask(mask);
	    needs_init_ = !bits_equal(force_, bit_);
	    ptr.ptr()->send_real(bit_, 0);
      } അല്ലെങ്കിൽ {
	      // Variables keep the current value.
	    ഇരട്ട res =  real_മൂല്യം();
	    release_mask(mask);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
}

ശൂന്യം vvp_wire_real::release_pv(vvp_net_ptr_t, unsigned, unsigned, തർക്കം)
{
      പ്രതിജ്ഞാ(0);
#എങ്കിൽ 0
	// A real is a single value. If for some reason this part release
	// can happen the following code should work correctly (requires
	// a base of 0 and a width of 1).
      vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
      പ്രതിജ്ഞാ(base == 0 && wid == 1);

      എങ്കിൽ (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_mask(mask);
	    ptr.ptr()->send_real(bit_, 0);
      } അല്ലെങ്കിൽ {
	      // Variables keep the current value.
	    ഇരട്ട res =  real_മൂല്യം();
	    release_mask(mask);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
#endif
}

unsigned vvp_wire_real::മൂല്യം_size() സ്ഥിരം
{
      പ്രതിജ്ഞാ(0);
      തിരികെ 1;
}

vvp_bit4_t vvp_wire_real::value(unsigned) സ്ഥിരം
{
      പ്രതിജ്ഞാ(0);
      തിരികെ BIT4_X;
}

vvp_scalar_t vvp_wire_real::scalar_മൂല്യം(unsigned) സ്ഥിരം
{
      പ്രതിജ്ഞാ(0);
      തിരികെ vvp_scalar_t();
}

ശൂന്യം vvp_wire_real::vec4_മൂല്യം(vvp_vector4_t&) സ്ഥിരം
{
      പ്രതിജ്ഞാ(0);
}

ഇരട്ട vvp_wire_real::real_മൂല്യം() സ്ഥിരം
{
      എങ്കിൽ (test_force_mask(0))
	    തിരികെ force_;
      അല്ലെങ്കിൽ
	    തിരികെ bit_;
}

#എങ്കിൽ 0
vvp_wire_string::vvp_wire_string()
{
}

unsigned vvp_wire_string::filter_size() സ്ഥിരം
{
      പ്രതിജ്ഞാ(0);
      തിരികെ 0;
}

ശൂന്യം vvp_wire_string::force_fil_vec4(സ്ഥിരം vvp_vector4_t&, സ്ഥിരം vvp_vector2_t&)
{
      പ്രതിജ്ഞാ(0);
}
ശൂന്യം vvp_wire_string::force_fil_vec8(സ്ഥിരം vvp_vector8_t&, സ്ഥിരം vvp_vector2_t&)
{
      പ്രതിജ്ഞാ(0);
}
ശൂന്യം vvp_wire_string::force_fil_real(ഇരട്ട, സ്ഥിരം vvp_vector2_t&)
{
      പ്രതിജ്ഞാ(0);
}

ശൂന്യം vvp_wire_string::release(vvp_net_ptr_t ptr, തർക്കം net_flag)
{
      പ്രതിജ്ഞാ(0);
}

ശൂന്യം vvp_wire_string::release_pv(vvp_net_ptr_t, unsigned, unsigned, തർക്കം)
{
      പ്രതിജ്ഞാ(0);
}

unsigned vvp_wire_string::മൂല്യം_size() സ്ഥിരം
{
      പ്രതിജ്ഞാ(0);
      തിരികെ 1;
}

vvp_bit4_t vvp_wire_string::value(unsigned) സ്ഥിരം
{
      പ്രതിജ്ഞാ(0);
      തിരികെ BIT4_X;
}

vvp_scalar_t vvp_wire_string::scalar_മൂല്യം(unsigned) സ്ഥിരം
{
      പ്രതിജ്ഞാ(0);
      തിരികെ vvp_scalar_t();
}

ശൂന്യം vvp_wire_string::vec4_മൂല്യം(vvp_vector4_t&) സ്ഥിരം
{
      പ്രതിജ്ഞാ(0);
}

ഇരട്ട vvp_wire_string::real_മൂല്യം() സ്ഥിരം
{
      പ്രതിജ്ഞാ(0);
      തിരികെ 0.0;
}
#endif
