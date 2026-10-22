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

using ନାମାକାଶ std;

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
template <ଶ୍ରେଣୀ T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_mask_(ଅଚଳ T&val, ଅଚଳ T&force, T&filter, unsigned base)
{
      ଯଦି (!test_force_mask_is_zero()) {
	      // Some bits are being forced. Go through the
	      // force_mask_ and force value to see which bits are
	      // propagated and which are kept from the forced
	      // value. Update the filter with the filtered result and
	      // return REPL to indicate that some bits have changed,
	      // or STOP if no bits change.
	    ତାର୍କିକ propagate_flag = force_propagate_;
	    force_propagate_ = ମିଥ୍ୟା;
	    ପ୍ରତିଜ୍ଞା(force_mask_.size() == force.size());
	    ପ୍ରତିଜ୍ଞା((base+val.size()) <= force_mask_.size());

	    filter = val;
	    ପାଇଁ (unsigned idx = 0 ; idx < val.size() ; idx += 1) {
		  ଯଦି (force_mask_.value(base+idx))
			filter.set_bit(idx, force.value(base+idx));
		  ଅନ୍ୟଥା
			propagate_flag = ସତ୍ୟ;
	    }

	    ଯଦି (propagate_flag) {
		  run_vpi_callbacks();
		  ଫେରନ୍ତୁ REPL;
	    } ଅନ୍ୟଥା {
		  ଫେରନ୍ତୁ STOP;
	    }

      } ଅନ୍ୟଥା {
	    run_vpi_callbacks();
	    ଫେରନ୍ତୁ PROP;
      }
}

template <ଶ୍ରେଣୀ T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_mask_(T&val, T force)
{

      ଯଦି (test_force_mask(0)) {
	    val = force;
	    run_vpi_callbacks();
	    ଫେରନ୍ତୁ REPL;
      }
      run_vpi_callbacks();
      ଫେରନ୍ତୁ PROP;
}

template <ଶ୍ରେଣୀ T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_ନିବେଶ_mask(ଅଚଳ T&val, ଅଚଳ T&force, T&rep) ଅଚଳ
{
      ଯଦି (test_force_mask_is_zero())
	    ଫେରନ୍ତୁ PROP;

      ପ୍ରତିଜ୍ଞା(force_mask_.size() == force.size());

      rep = val;
      ପାଇଁ (unsigned idx = 0 ; idx < val.size() ; idx += 1) {
	    ଯଦି (force_mask_.value(idx))
		  rep.set_bit(idx, force.value(idx));
      }

      ଫେରନ୍ତୁ REPL;
}

vvp_signal_ମୂଲ୍ୟ::~vvp_signal_ମୂଲ୍ୟ()
{
}

ଦ୍ୱିଗୁଣ vvp_signal_ମୂଲ୍ୟ::real_ମୂଲ୍ୟ() ଅଚଳ
{
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ vvp_net_t::force_vec4(ଅଚଳ vvp_vector4_t&val, ଅଚଳ vvp_vector2_t&mask)
{
      ପ୍ରତିଜ୍ଞା(fil);
      fil->force_fil_vec4(val, mask);
      fun->force_flag(ମିଥ୍ୟା);
      vvp_send_vec4(out_, val, 0);
}

ଶୂନ୍ୟ vvp_net_t::force_vec8(ଅଚଳ vvp_vector8_t&val, ଅଚଳ vvp_vector2_t&mask)
{
      ପ୍ରତିଜ୍ଞା(fil);
      fil->force_fil_vec8(val, mask);
      fun->force_flag(ମିଥ୍ୟା);
      vvp_send_vec8(out_, val);
}

ଶୂନ୍ୟ vvp_net_t::force_real(ଦ୍ୱିଗୁଣ val, ଅଚଳ vvp_vector2_t&mask)
{
      ପ୍ରତିଜ୍ଞା(fil);
      fil->force_fil_real(val, mask);
      fun->force_flag(ମିଥ୍ୟା);
      vvp_send_real(out_, val, 0);
}

/* **** vvp_fun_signal methods **** */

vvp_fun_signal_base::vvp_fun_signal_base()
{
      continuous_assign_active_ = ମିଥ୍ୟା;
      needs_init_ = ସତ୍ୟ;
      cassign_link = 0;
      ଗଣନା_functors_sig += 1;
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
ଶୂନ୍ୟ vvp_fun_signal4_sa::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                                   vvp_context_t)
{
      ବାଛ (ptr.port()) {
	  ସ୍ଥିତି 0: // Normal input (feed from net, or set from process)
	      /* If we don't have a continuous assign mask then just
		 copy the bits, otherwise we need to see if there are
		 any holes in the mask so we can set those bits. */
	    ଯଦି (assign_mask_.size() == 0) {
                  ଯଦି (needs_init_ || !bits4_.eeq(bit)) {
			ପ୍ରତିଜ୍ଞା(bit.size() == bits4_.size());
			bits4_ = bit;
			needs_init_ = ମିଥ୍ୟା;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    } ଅନ୍ୟଥା {
		  ତାର୍କିକ changed = ମିଥ୍ୟା;
		  ପ୍ରତିଜ୍ଞା(bits4_.size() == assign_mask_.size());
		  ପାଇଁ (unsigned idx = 0 ;  idx < bit.size() ;  idx += 1) {
			ଯଦି (idx >= bits4_.size()) ଭାଙ୍ଗ;
			ଯଦି (assign_mask_.value(idx)) ଜାରି;
			bits4_.set_bit(idx, bit.value(idx));
			changed = ସତ୍ୟ;
		  }
		  ଯଦି (changed) {
			needs_init_ = ମିଥ୍ୟା;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    }
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି 1: // Continuous assign value
	      // Handle the simple case of the linked source being wider
	      // than this signal. Note we don't yet support the case of
	      // the linked source being narrower than this signal, or
	      // the case of an expression being assigned.
	    bits4_ = coerce_to_width(bit, bits4_.size());
	    assign_mask_ = vvp_vector2_t(vvp_vector2_t::FILL1, bits4_.size());
	    ptr.ptr()->send_vec4(bits4_, 0);
	    ଭାଙ୍ଗ;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    ପ୍ରତିଜ୍ଞା(0);
	    ଭାଙ୍ଗ;
      }
}

ଶୂନ୍ୟ vvp_fun_signal4_sa::recv_vec8(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector8_t&bit)
{
      recv_vec4(ptr, reduce4(bit), 0);
}

ଶୂନ୍ୟ vvp_fun_signal4_sa::recv_vec4_pv(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
				      unsigned base, unsigned vwid, vvp_context_t)
{
      ପ୍ରତିଜ୍ଞା(bits4_.size() == vwid);
      unsigned wid = bit.size();

      ବାଛ (ptr.port()) {
	  ସ୍ଥିତି 0: // Normal input
	    ଯଦି (assign_mask_.size() == 0) {
                  ପାଇଁ (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
			ଯଦି (base+idx >= bits4_.size()) ଭାଙ୍ଗ;
			bits4_.set_bit(base+idx, bit.value(idx));
		  }
		  needs_init_ = ମିଥ୍ୟା;
		  ptr.ptr()->send_vec4(bits4_,0);
	    } ଅନ୍ୟଥା {
		  ତାର୍କିକ changed = ମିଥ୍ୟା;
		  ପ୍ରତିଜ୍ଞା(bits4_.size() == assign_mask_.size());
		  ପାଇଁ (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
			ଯଦି (base+idx >= bits4_.size()) ଭାଙ୍ଗ;
			ଯଦି (assign_mask_.value(base+idx)) ଜାରି;
			bits4_.set_bit(base+idx, bit.value(idx));
			changed = ସତ୍ୟ;
		  }
		  ଯଦି (changed) {
			needs_init_ = ମିଥ୍ୟା;
			ptr.ptr()->send_vec4(bits4_,0);
		  }
	    }
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି 1: // Continuous assign value
	    ଯଦି (assign_mask_.size() == 0)
		  assign_mask_ = vvp_vector2_t(vvp_vector2_t::FILL0, bits4_.size());
	    ପାଇଁ (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  ଯଦି (base+idx >= bits4_.size())
			ଭାଙ୍ଗ;
		  bits4_.set_bit(base+idx, bit.value(idx));
		  assign_mask_.set_bit(base+idx, 1);
	    }
	    ptr.ptr()->send_vec4(bits4_,0);
	    ଭାଙ୍ଗ;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    ପ୍ରତିଜ୍ଞା(0);
	    ଭାଙ୍ଗ;
      }
}

ଶୂନ୍ୟ vvp_fun_signal4_sa::recv_vec8_pv(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector8_t&bit,
				      unsigned base, unsigned vwid)
{
      recv_vec4_pv(ptr, reduce4(bit), base, vwid, 0);
}

ଶୂନ୍ୟ vvp_fun_signal_base::deassign()
{
      continuous_assign_active_ = ମିଥ୍ୟା;
      assign_mask_ = vvp_vector2_t();
}

ଶୂନ୍ୟ vvp_fun_signal_base::deassign_pv(unsigned base, unsigned wid)
{
      ପାଇଁ (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    assign_mask_.set_bit(base+idx, 0);
      }

      ଯଦି (assign_mask_.is_zero()) {
	    assign_mask_ = vvp_vector2_t();
      }
}

ଶୂନ୍ୟ automatic_signal_base::release(vvp_net_ptr_t,ତାର୍କିକ)
{
      ପ୍ରତିଜ୍ଞା(0);
}

ଶୂନ୍ୟ automatic_signal_base::release_pv(vvp_net_ptr_t,unsigned,unsigned,ତାର୍କିକ)
{
      ପ୍ରତିଜ୍ଞା(0);
}

unsigned automatic_signal_base::filter_size() ଅଚଳ
{
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ(0);
}
ଶୂନ୍ୟ automatic_signal_base::force_fil_vec4(ଅଚଳ vvp_vector4_t&, ଅଚଳ vvp_vector2_t&)
{
      ପ୍ରତିଜ୍ଞା(0);
}
ଶୂନ୍ୟ automatic_signal_base::force_fil_vec8(ଅଚଳ vvp_vector8_t&, ଅଚଳ vvp_vector2_t&)
{
      ପ୍ରତିଜ୍ଞା(0);
}
ଶୂନ୍ୟ automatic_signal_base::force_fil_real(ଦ୍ୱିଗୁଣ, ଅଚଳ vvp_vector2_t&)
{
      ପ୍ରତିଜ୍ଞା(0);
}
ଶୂନ୍ୟ automatic_signal_base::get_ମୂଲ୍ୟ(ସଂରଚନା t_vpi_ମୂଲ୍ୟ*)
{
      ପ୍ରତିଜ୍ଞା(0);
}

ଅଚଳ vvp_vector4_t& vvp_fun_signal4_sa::vec4_unfiltered_ମୂଲ୍ୟ() ଅଚଳ
{
      ଫେରନ୍ତୁ bits4_;
}

vvp_fun_signal4_aa::vvp_fun_signal4_aa(unsigned wid, vvp_bit4_t init)
{
      context_idx_ = vpip_add_item_to_context(ଏହି, vpip_peek_context_scope());
      size_ = wid;
      init_ = init;
}

vvp_fun_signal4_aa::~vvp_fun_signal4_aa()
{
      ପ୍ରତିଜ୍ଞା(0);
}

ଶୂନ୍ୟ vvp_fun_signal4_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, ନୂତନ vvp_vector4_t(size_, init_));
}

ଶୂନ୍ୟ vvp_fun_signal4_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*bits = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      bits->fill_bits(init_);
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ vvp_fun_signal4_aa::free_instance(vvp_context_t context)
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
ଶୂନ୍ୟ vvp_fun_signal4_aa::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      ପ୍ରତିଜ୍ଞା(ptr.port() == 0);
      ପ୍ରତିଜ୍ଞା(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      ଯଦି (!bits4->eeq(bit)) {
            *bits4 = bit;
            ptr.ptr()->send_vec4(*bits4, context);
      }
}

ଶୂନ୍ୟ vvp_fun_signal4_aa::recv_vec4_pv(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
				      unsigned base, unsigned vwid, vvp_context_t context)
{
      ପ୍ରତିଜ୍ଞା(ptr.port() == 0);
      ପ୍ରତିଜ୍ଞା(size_ == vwid);
      ପ୍ରତିଜ୍ଞା(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      unsigned wid = bit.size();
      ପାଇଁ (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
            ଯଦି (base+idx >= bits4->size()) ଭାଙ୍ଗ;
            bits4->set_bit(base+idx, bit.value(idx));
      }
      ptr.ptr()->send_vec4(*bits4, context);
}

unsigned vvp_fun_signal4_aa::ମୂଲ୍ୟ_size() ଅଚଳ
{
      ଫେରନ୍ତୁ size_;
}

vvp_bit4_t vvp_fun_signal4_aa::value(unsigned idx) ଅଚଳ
{
      ଅଚଳ vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      ଫେରନ୍ତୁ bits4->value(idx);
}

vvp_scalar_t vvp_fun_signal4_aa::scalar_ମୂଲ୍ୟ(unsigned idx) ଅଚଳ
{
      ଅଚଳ vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      ଫେରନ୍ତୁ vvp_scalar_t(bits4->value(idx), 6, 6);
}

ଶୂନ୍ୟ vvp_fun_signal4_aa::vec4_ମୂଲ୍ୟ(vvp_vector4_t&val) ଅଚଳ
{
      ଅଚଳ vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      val = *bits4;
}

ଅଚଳ vvp_vector4_t&vvp_fun_signal4_aa::vec4_unfiltered_ମୂଲ୍ୟ() ଅଚଳ
{
      ଅଚଳ vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      ଫେରନ୍ତୁ *bits4;
}

ଶୂନ୍ୟ vvp_fun_signal4_aa::operator delete(ଶୂନ୍ୟ*)
{
      ପ୍ରତିଜ୍ଞା(0);
}

/*
 * Testing for equality, we want a bitwise test instead of an
 * arithmetic test because we want to treat for example -0 different
 * from +0.
 */
ତାର୍କିକ bits_equal(ଦ୍ୱିଗୁଣ a, ଦ୍ୱିଗୁଣ b)
{
      ଫେରନ୍ତୁ memcmp(&a, &b, ଆକାର a) == 0;
}

vvp_fun_signal_real_sa::vvp_fun_signal_real_sa()
{
      bits_ = 0.0;
}

ଦ୍ୱିଗୁଣ vvp_fun_signal_real_sa::real_unfiltered_ମୂଲ୍ୟ() ଅଚଳ
{
      ଫେରନ୍ତୁ bits_;
}

ଶୂନ୍ୟ vvp_fun_signal_real_sa::recv_real(vvp_net_ptr_t ptr, ଦ୍ୱିଗୁଣ bit,
                                       vvp_context_t)
{
      ବାଛ (ptr.port()) {
	  ସ୍ଥିତି 0:
	    ଯଦି (!continuous_assign_active_) {
                  ଯଦି (needs_init_ || !bits_equal(bits_, bit)) {
			bits_ = bit;
			needs_init_ = ମିଥ୍ୟା;
			ptr.ptr()->send_real(bit, 0);
		  }
	    }
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି 1: // Continuous assign value
	    continuous_assign_active_ = ସତ୍ୟ;
	    bits_ = bit;
	    ptr.ptr()->send_real(bit, 0);
	    ଭାଙ୍ଗ;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    ପ୍ରତିଜ୍ଞା(0);
	    ଭାଙ୍ଗ;
      }
}

vvp_fun_signal_real_aa::vvp_fun_signal_real_aa()
{
      context_idx_ = vpip_add_item_to_context(ଏହି, vpip_peek_context_scope());
}

vvp_fun_signal_real_aa::~vvp_fun_signal_real_aa()
{
      ପ୍ରତିଜ୍ଞା(0);
}

ଶୂନ୍ୟ vvp_fun_signal_real_aa::alloc_instance(vvp_context_t context)
{
      ଦ୍ୱିଗୁଣ*bits = ନୂତନ ଦ୍ୱିଗୁଣ;
      vvp_set_context_item(context, context_idx_, bits);

      *bits = 0.0;
}

ଶୂନ୍ୟ vvp_fun_signal_real_aa::reset_instance(vvp_context_t context)
{
      ଦ୍ୱିଗୁଣ*bits = static_cast<ଦ୍ୱିଗୁଣ*>
            (vvp_get_context_item(context, context_idx_));

      *bits = 0.0;
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ vvp_fun_signal_real_aa::free_instance(vvp_context_t context)
{
      ଦ୍ୱିଗୁଣ*bits = static_cast<ଦ୍ୱିଗୁଣ*>
            (vvp_get_context_item(context, context_idx_));
      delete bits;
}
#endif

ଦ୍ୱିଗୁଣ vvp_fun_signal_real_aa::real_unfiltered_ମୂଲ୍ୟ() ଅଚଳ
{
      ଅଚଳ ଦ୍ୱିଗୁଣ*bits = static_cast<ଦ୍ୱିଗୁଣ*>
            (vthread_get_rd_context_item(context_idx_));

      ଫେରନ୍ତୁ *bits;
}

ଦ୍ୱିଗୁଣ vvp_fun_signal_real_aa::real_ମୂଲ୍ୟ() ଅଚଳ
{
      ଫେରନ୍ତୁ real_unfiltered_ମୂଲ୍ୟ();
}

ଶୂନ୍ୟ vvp_fun_signal_real_aa::recv_real(vvp_net_ptr_t ptr, ଦ୍ୱିଗୁଣ bit,
                                       vvp_context_t context)
{
      ପ୍ରତିଜ୍ଞା(ptr.port() == 0);
      ପ୍ରତିଜ୍ଞା(context);

      ଦ୍ୱିଗୁଣ*bits = static_cast<ଦ୍ୱିଗୁଣ*>
            (vvp_get_context_item(context, context_idx_));

      ଯଦି (!bits_equal(*bits,bit)) {
            *bits = bit;
            ptr.ptr()->send_real(bit, context);
      }
}

unsigned vvp_fun_signal_real_aa::ମୂଲ୍ୟ_size() ଅଚଳ
{
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ 1;
}

vvp_bit4_t vvp_fun_signal_real_aa::value(unsigned) ଅଚଳ
{
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ BIT4_X;
}

vvp_scalar_t vvp_fun_signal_real_aa::scalar_ମୂଲ୍ୟ(unsigned) ଅଚଳ
{
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ vvp_scalar_t();
}

ଶୂନ୍ୟ vvp_fun_signal_real_aa::vec4_ମୂଲ୍ୟ(vvp_vector4_t&) ଅଚଳ
{
      ପ୍ରତିଜ୍ଞା(0);
}

ଶୂନ୍ୟ* vvp_fun_signal_real_aa::operator ନୂତନ(std::size_t size)
{
      ଫେରନ୍ତୁ vvp_net_fun_t::heap_.alloc(size);
}

ଶୂନ୍ୟ vvp_fun_signal_real_aa::operator delete(ଶୂନ୍ୟ*)
{
      ପ୍ରତିଜ୍ଞା(0);
}


vvp_fun_signal_string_sa::vvp_fun_signal_string_sa()
{
}

ଶୂନ୍ୟ vvp_fun_signal_string_sa::recv_string(vvp_net_ptr_t ptr, ଅଚଳ std::string&bit,
					   vvp_context_t)
{
      ପ୍ରତିଜ୍ଞା(ptr.port() == 0);

      ଯଦି (needs_init_ || ମୂଲ୍ୟ_2 != bit) {
	    ମୂଲ୍ୟ_2 = bit;
	    needs_init_ = ମିଥ୍ୟା;

	    ptr.ptr()->send_string(bit, 0);
      }
}

ଅଚଳ string& vvp_fun_signal_string_sa::get_string() ଅଚଳ
{
      ଫେରନ୍ତୁ ମୂଲ୍ୟ_2;
}

vvp_fun_signal_string_aa::vvp_fun_signal_string_aa()
{
      context_idx_ = vpip_add_item_to_context(ଏହି, vpip_peek_context_scope());
}

vvp_fun_signal_string_aa::~vvp_fun_signal_string_aa()
{
      ପ୍ରତିଜ୍ଞା(0);
}

ଶୂନ୍ୟ vvp_fun_signal_string_aa::alloc_instance(vvp_context_t context)
{
      string*bits = ନୂତନ std::string;
      vvp_set_context_item(context, context_idx_, bits);
      *bits = "";
}

ଶୂନ୍ୟ vvp_fun_signal_string_aa::reset_instance(vvp_context_t context)
{
      string*bits = static_cast<std::string*>
	    (vvp_get_context_item(context, context_idx_));
      *bits = "";
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ vvp_fun_signal_string_aa::free_instance(vvp_context_t context)
{
      string*bits = static_cast<std::string*>
            (vvp_get_context_item(context, context_idx_));
      delete bits;
}
#endif

ଶୂନ୍ୟ vvp_fun_signal_string_aa::recv_string(vvp_net_ptr_t ptr, ଅଚଳ std::string&bit, vvp_context_t context)
{
      ପ୍ରତିଜ୍ଞା(ptr.port() == 0);
      ପ୍ରତିଜ୍ଞା(context);

      string*bits = static_cast<std::string*>
	    (vvp_get_context_item(context, context_idx_));

      ଯଦି (*bits != bit) {
	    *bits = bit;
	    ptr.ptr()->send_string(bit, context);
      }
}

unsigned vvp_fun_signal_string_aa::ମୂଲ୍ୟ_size() ଅଚଳ
{
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ 1;
}

vvp_bit4_t vvp_fun_signal_string_aa::value(unsigned) ଅଚଳ
{
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ BIT4_X;
}

vvp_scalar_t vvp_fun_signal_string_aa::scalar_ମୂଲ୍ୟ(unsigned) ଅଚଳ
{
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ vvp_scalar_t();
}

ଶୂନ୍ୟ vvp_fun_signal_string_aa::vec4_ମୂଲ୍ୟ(vvp_vector4_t&) ଅଚଳ
{
      ପ୍ରତିଜ୍ଞା(0);
}

ଦ୍ୱିଗୁଣ vvp_fun_signal_string_aa::real_ମୂଲ୍ୟ() ଅଚଳ
{
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ 0.0;
}

ଅଚଳ std::string& vvp_fun_signal_string_aa::get_string() ଅଚଳ
{
      ଅଚଳ string*bits = static_cast<std::string*>
            (vthread_get_rd_context_item(context_idx_));

      ଫେରନ୍ତୁ *bits;
}

ଶୂନ୍ୟ* vvp_fun_signal_string_aa::operator ନୂତନ(std::size_t size)
{
      ଫେରନ୍ତୁ vvp_net_fun_t::heap_.alloc(size);
}

ଶୂନ୍ୟ vvp_fun_signal_string_aa::operator delete(ଶୂନ୍ୟ*)
{
      ପ୍ରତିଜ୍ଞା(0);
}

  /* OBJECT signals */

vvp_fun_signal_object_sa::vvp_fun_signal_object_sa(unsigned size)
: vvp_fun_signal_object(size)
{
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ vvp_fun_signal_object_aa::free_instance(vvp_context_t context)
{
      vvp_object_t*bits = static_cast<vvp_object_t*>
            (vvp_get_context_item(context, context_idx_));
      delete bits;
}
#endif

ଶୂନ୍ୟ vvp_fun_signal_object_sa::recv_object(vvp_net_ptr_t ptr, vvp_object_t bit,
					   vvp_context_t)
{
      ପ୍ରତିଜ୍ଞା(ptr.port() == 0);

      ଯଦି (needs_init_ || ମୂଲ୍ୟ_2 != bit) {
	    ମୂଲ୍ୟ_2 = bit;
	    needs_init_ = ମିଥ୍ୟା;

	    ptr.ptr()->send_object(bit, 0);
      }
}

vvp_object_t vvp_fun_signal_object_sa::get_object() ଅଚଳ
{
      ଫେରନ୍ତୁ ମୂଲ୍ୟ_2;
}

vvp_fun_signal_object_aa::vvp_fun_signal_object_aa(unsigned size)
: vvp_fun_signal_object(size)
{
      context_idx_ = vpip_add_item_to_context(ଏହି, vpip_peek_context_scope());
}

vvp_fun_signal_object_aa::~vvp_fun_signal_object_aa()
{
      ପ୍ରତିଜ୍ଞା(0);
}

ଶୂନ୍ୟ vvp_fun_signal_object_aa::alloc_instance(vvp_context_t context)
{
      vvp_object_t*bits = ନୂତନ vvp_object_t;
      vvp_set_context_item(context, context_idx_, bits);
      bits->reset();
}

ଶୂନ୍ୟ vvp_fun_signal_object_aa::reset_instance(vvp_context_t context)
{
      vvp_object_t*bits = static_cast<vvp_object_t*>
	    (vvp_get_context_item(context, context_idx_));
      bits->reset();
}

vvp_object_t vvp_fun_signal_object_aa::get_object() ଅଚଳ
{
      ଅଚଳ vvp_object_t*bits = static_cast<vvp_object_t*>
	    (vthread_get_rd_context_item(context_idx_));
      ଫେରନ୍ତୁ *bits;
}

ଶୂନ୍ୟ vvp_fun_signal_object_aa::recv_object(vvp_net_ptr_t ptr, vvp_object_t bit,
					   vvp_context_t context)
{
      ପ୍ରତିଜ୍ଞା(ptr.port() == 0);
      ପ୍ରତିଜ୍ଞା(context);

      vvp_object_t*bits = static_cast<vvp_object_t*>
	    (vvp_get_context_item(context, context_idx_));

      ଯଦି (*bits != bit) {
	    *bits = bit;
	    ptr.ptr()->send_object(bit, context);
      }
}

unsigned vvp_fun_signal_object_aa::ମୂଲ୍ୟ_size() ଅଚଳ
{
      ଫେରନ୍ତୁ 1;
}

vvp_bit4_t vvp_fun_signal_object_aa::value(unsigned) ଅଚଳ
{
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ BIT4_X;
}

vvp_scalar_t vvp_fun_signal_object_aa::scalar_ମୂଲ୍ୟ(unsigned) ଅଚଳ
{
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ vvp_scalar_t();
}

ଶୂନ୍ୟ vvp_fun_signal_object_aa::vec4_ମୂଲ୍ୟ(vvp_vector4_t&) ଅଚଳ
{
      ପ୍ରତିଜ୍ଞା(0);
}

ଶୂନ୍ୟ* vvp_fun_signal_object_aa::operator ନୂତନ(std::size_t size)
{
      ଫେରନ୍ତୁ vvp_net_fun_t::heap_.alloc(size);
}

ଶୂନ୍ୟ vvp_fun_signal_object_aa::operator delete(ଶୂନ୍ୟ*)
{
      ପ୍ରତିଜ୍ଞା(0);
}

  /* **** */

vvp_fun_force::vvp_fun_force()
{
}

vvp_fun_force::~vvp_fun_force()
{
}

ଶୂନ୍ୟ vvp_fun_force::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
			      vvp_context_t)
{
      ପ୍ରତିଜ୍ଞା(ptr.port() == 0);
      vvp_net_t*net = ptr.ptr();

      vvp_net_t*dst = net->port[3].ptr();
      ପ୍ରତିଜ୍ଞା(dst->fil);

      dst->force_vec4(coerce_to_width(bit, dst->fil->filter_size()), vvp_vector2_t(vvp_vector2_t::FILL1, dst->fil->filter_size()));
}

ଶୂନ୍ୟ vvp_fun_force::recv_real(vvp_net_ptr_t ptr, ଦ୍ୱିଗୁଣ bit, vvp_context_t)
{
      ପ୍ରତିଜ୍ଞା(ptr.port() == 0);
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

vvp_bit4_t vvp_wire_base::driven_ମୂଲ୍ୟ(unsigned) ଅଚଳ
{
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ BIT4_X;
}

ତାର୍କିକ vvp_wire_base::is_forced(unsigned) ଅଚଳ
{
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

vvp_wire_vec4::vvp_wire_vec4(unsigned wid, vvp_bit4_t init)
: bits4_(wid, init)
{
      needs_init_ = ସତ୍ୟ;
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec4(ଅଚଳ vvp_vector4_t&bit, vvp_vector4_t&rep,
						 unsigned base, unsigned vwid)
{
	// Special case! the input bit is 0 wid. Interpret this as a
	// vector of BIT4_X to match the width of the bits4_ vector.
	// FIXME! This is a hack to work around some buggy gate
	// implementations! This should be removed!
      ଯଦି (base==0 && vwid==0) {
	    vvp_vector4_t tmp (bits4_.size(), BIT4_X);
	    ଯଦି (bits4_ .eeq(tmp) && !needs_init_) ଫେରନ୍ତୁ STOP;
	    bits4_ = tmp;
	    needs_init_ = ମିଥ୍ୟା;
	    ଫେରନ୍ତୁ filter_mask_(tmp, force4_, rep, 0);
      }

      ଯଦି (vwid != bits4_.size()) {
	    cerr << "Internal error: Input vector expected width="
		 << bits4_.size() << ", got "
		 << "bit=" << bit << ", base=" << base << ", vwid=" << vwid
		 << endl;
      }
      ପ୍ରତିଜ୍ଞା(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      ଯଦି (base==0 && bit.size()==vwid) {
	    ଯଦି (bits4_ .eeq( bit ) && !needs_init_) ଫେରନ୍ତୁ STOP;
	    bits4_ = bit;
      } ଅନ୍ୟଥା {
	    ତାର୍କିକ rc = bits4_.set_vec(base, bit);
	    ଯଦି (rc == ମିଥ୍ୟା && !needs_init_) ଫେରନ୍ତୁ STOP;
      }

      needs_init_ = ମିଥ୍ୟା;
      ଫେରନ୍ତୁ filter_mask_(bit, force4_, rep, base);
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec8(ଅଚଳ vvp_vector8_t&bit,
                                                 vvp_vector8_t&rep,
                                                 unsigned base,
                                                 unsigned vwid)
{
      ପ୍ରତିଜ୍ଞା(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      vvp_vector4_t bit4 (reduce4(bit));
      ଯଦି (base==0 && bit4.size()==vwid) {
	    ଯଦି (bits4_ .eeq( bit4 ) && !needs_init_) ଫେରନ୍ତୁ STOP;
	    bits4_ = bit4;
      } ଅନ୍ୟଥା {
	    ତାର୍କିକ rc = bits4_.set_vec(base, bit4);
	    ଯଦି (rc == ମିଥ୍ୟା && !needs_init_) ଫେରନ୍ତୁ STOP;
      }

      needs_init_ = ମିଥ୍ୟା;
      ଫେରନ୍ତୁ filter_mask_(bit, vvp_vector8_t(force4_,6,6), rep, base);
}

unsigned vvp_wire_vec4::filter_size() ଅଚଳ
{
      ଫେରନ୍ତୁ bits4_.size();
}

ଶୂନ୍ୟ vvp_wire_vec4::force_fil_vec4(ଅଚଳ vvp_vector4_t&val, ଅଚଳ vvp_vector2_t&mask)
{
      force_mask(mask);

      ଯଦି (force4_.size() == 0) {
	    force4_ = val;
      } ଅନ୍ୟଥା {
	    ପାଇଁ (unsigned idx = 0; idx < mask.size() ; idx += 1) {
		  ଯଦି (mask.value(idx) == 0)
			ଜାରି;

		  force4_.set_bit(idx, val.value(idx));
	    }
      }
      run_vpi_callbacks();
}

ଶୂନ୍ୟ vvp_wire_vec4::force_fil_vec8(ଅଚଳ vvp_vector8_t&, ଅଚଳ vvp_vector2_t&)
{
      ପ୍ରତିଜ୍ଞା(0);
}

ଶୂନ୍ୟ vvp_wire_vec4::force_fil_real(ଦ୍ୱିଗୁଣ, ଅଚଳ vvp_vector2_t&)
{
      ପ୍ରତିଜ୍ଞା(0);
}

ଶୂନ୍ୟ vvp_wire_vec4::release(vvp_net_ptr_t ptr, ତାର୍କିକ net_flag)
{
      vvp_vector2_t mask (vvp_vector2_t::FILL1, bits4_.size());
      ଯଦି (net_flag) {
	      // Wires revert to their unforced value after release.
            release_mask(mask);
	    needs_init_ = ! force4_ .eeq(bits4_);
	    ptr.ptr()->send_vec4(bits4_, 0);
	    run_vpi_callbacks();
      } ଅନ୍ୟଥା {
	      // Variables keep the current value.
	    vvp_vector4_t res (bits4_.size());
	    ପାଇଁ (unsigned idx=0; idx<bits4_.size(); idx += 1)
		  res.set_bit(idx,value(idx));
            release_mask(mask);
	    ptr.ptr()->fun->recv_vec4(ptr, res, 0);
      }
}

ଶୂନ୍ୟ vvp_wire_vec4::release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, ତାର୍କିକ net_flag)
{
      ପ୍ରତିଜ୍ଞା(bits4_.size() >= base + wid);

      vvp_vector2_t mask (vvp_vector2_t::FILL0, bits4_.size());
      ପାଇଁ (unsigned idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      ଯଦି (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_mask(mask);
	    needs_init_ = ! force4_.subvalue(base,wid) .eeq(bits4_.subvalue(base,wid));
	    ptr.ptr()->send_vec4_pv(bits4_.subvalue(base,wid),
				    base, bits4_.size(), 0);
	    run_vpi_callbacks();
      } ଅନ୍ୟଥା {
	      // Variables keep the current value.
	    vvp_vector4_t res (wid);
	    ପାଇଁ (unsigned idx=0; idx<wid; idx += 1)
		  res.set_bit(idx,value(base+idx));
	    release_mask(mask);
	    ptr.ptr()->fun->recv_vec4_pv(ptr, res, base, bits4_.size(), 0);
      }
}

unsigned vvp_wire_vec4::ମୂଲ୍ୟ_size() ଅଚଳ
{
      ଫେରନ୍ତୁ bits4_.size();
}

vvp_bit4_t vvp_wire_vec4::filtered_ମୂଲ୍ୟ(unsigned idx) ଅଚଳ
{
      ଯଦି (test_force_mask(idx))
	    ଫେରନ୍ତୁ force4_.value(idx);
      ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ bits4_.value(idx);
}

vvp_bit4_t vvp_wire_vec4::value(unsigned idx) ଅଚଳ
{
      ଫେରନ୍ତୁ filtered_ମୂଲ୍ୟ(idx);
}

vvp_scalar_t vvp_wire_vec4::scalar_ମୂଲ୍ୟ(unsigned idx) ଅଚଳ
{
      ଫେରନ୍ତୁ vvp_scalar_t(value(idx),6,6);
}

ଶୂନ୍ୟ vvp_wire_vec4::vec4_ମୂଲ୍ୟ(vvp_vector4_t&val) ଅଚଳ
{
      val = bits4_;
      ଯଦି (test_force_mask_is_zero())
	    ଫେରନ୍ତୁ;

      ପାଇଁ (unsigned idx = 0 ; idx < bits4_.size() ; idx += 1)
	    val.set_bit(idx, filtered_ମୂଲ୍ୟ(idx));
}

vvp_bit4_t vvp_wire_vec4::driven_ମୂଲ୍ୟ(unsigned idx) ଅଚଳ
{
      ଫେରନ୍ତୁ bits4_.value(idx);
}

ତାର୍କିକ vvp_wire_vec4::is_forced(unsigned idx) ଅଚଳ
{
      ଫେରନ୍ତୁ test_force_mask(idx);
}

vvp_wire_vec8::vvp_wire_vec8(unsigned wid)
: bits8_(wid)
{
      needs_init_ = ସତ୍ୟ;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec4(ଅଚଳ vvp_vector4_t&bit,
                                                 vvp_vector4_t&rep,
                                                 unsigned base,
                                                 unsigned vwid)
{
	// For now there is no support for a non-zero base.
      ପ୍ରତିଜ୍ଞା(0 == base);
      ପ୍ରତିଜ୍ଞା(bits8_.size() == vwid);
      ପ୍ରତିଜ୍ଞା(bits8_.size() == bit.size());
	// QUESTION: Is it really correct to propagate a vec4 if this
	// is a vec8 node? In fact, it is really possible for a vec4
	// value to get through to a vec8 filter?
      vvp_vector8_t rep8;
      prop_t rc = filter_vec8(vvp_vector8_t(bit,6,6), rep8, 0, vwid);
      ଯଦି (rc == REPL)
	    rep = reduce4(rep8);

      needs_init_ = ମିଥ୍ୟା;
      ଫେରନ୍ତୁ rc;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec8(ଅଚଳ vvp_vector8_t&bit, vvp_vector8_t&rep, unsigned base, unsigned vwid)
{
      ପ୍ରତିଜ୍ଞା(vwid == bits8_.size());
	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      ଯଦି (base==0 && bit.size()==vwid) {
	    ଯଦି (bits8_.eeq(bit) && !needs_init_) ଫେରନ୍ତୁ STOP;
	    bits8_ = bit;
      } ଅନ୍ୟଥା {
	    ଯଦି (bits8_.subvalue(base, bit.size()).eeq(bit) &&
		!needs_init_) {
		  ଫେରନ୍ତୁ STOP;
	    }
	    ଯଦି (bits8_.size() == 0)
		  bits8_ = vvp_vector8_t(vwid);
	    ପ୍ରତିଜ୍ଞା(bits8_.size() == vwid);
	    bits8_.set_vec(base, bit);
      }
      needs_init_ = ମିଥ୍ୟା;
      ଫେରନ୍ତୁ filter_mask_(bit, force8_, rep, base);
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_ନିବେଶ_vec8(ଅଚଳ vvp_vector8_t&bit, vvp_vector8_t&rep) ଅଚଳ
{
      ଫେରନ୍ତୁ filter_ନିବେଶ_mask(bit, force8_, rep);
}

unsigned vvp_wire_vec8::filter_size() ଅଚଳ
{
      ଫେରନ୍ତୁ bits8_.size();
}

ଶୂନ୍ୟ vvp_wire_vec8::force_fil_vec4(ଅଚଳ vvp_vector4_t&val, ଅଚଳ vvp_vector2_t&mask)
{
      force_fil_vec8(vvp_vector8_t(val,6,6), mask);
}

ଶୂନ୍ୟ vvp_wire_vec8::force_fil_vec8(ଅଚଳ vvp_vector8_t&val, ଅଚଳ vvp_vector2_t&mask)
{
      force_mask(mask);

      ଯଦି (force8_.size() == 0) {
	    force8_ = val;
      } ଅନ୍ୟଥା {
	    ପାଇଁ (unsigned idx = 0; idx < mask.size() ; idx += 1) {
		  ଯଦି (mask.value(idx) == 0)
			ଜାରି;

		  force8_.set_bit(idx, val.value(idx));
	    }
      }
      run_vpi_callbacks();
}

ଶୂନ୍ୟ vvp_wire_vec8::force_fil_real(ଦ୍ୱିଗୁଣ, ଅଚଳ vvp_vector2_t&)
{
      ପ୍ରତିଜ୍ଞା(0);
}

ଶୂନ୍ୟ vvp_wire_vec8::release(vvp_net_ptr_t ptr, ତାର୍କିକ net_flag)
{
	// Wires revert to their unforced value after release.
      vvp_vector2_t mask (vvp_vector2_t::FILL1, bits8_.size());
      release_mask(mask);
      ଯଦି (net_flag) {
	    needs_init_ = !force8_ .eeq(bits8_);
	    ptr.ptr()->send_vec8(bits8_);
      } ଅନ୍ୟଥା {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    ପ୍ରତିଜ୍ଞା(0);
//	    ptr.ptr()->fun->recv_vec8(ptr, force8_);
      }
}

ଶୂନ୍ୟ vvp_wire_vec8::release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, ତାର୍କିକ net_flag)
{
      ପ୍ରତିଜ୍ଞା(bits8_.size() >= base + wid);

      vvp_vector2_t mask (vvp_vector2_t::FILL0, bits8_.size());
      ପାଇଁ (unsigned idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      release_mask(mask);

      ଯଦି (net_flag) {
	    needs_init_ = !force8_.subvalue(base,wid) .eeq((bits8_.subvalue(base,wid)));
	    ptr.ptr()->send_vec8_pv(bits8_.subvalue(base,wid),
				    base, bits8_.size());
	    run_vpi_callbacks();
      } ଅନ୍ୟଥା {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    ପ୍ରତିଜ୍ଞା(0);
//	    ptr.ptr()->fun->recv_vec8_pv(ptr, force8_.subvalue(base,wid),
//					 base, force8_.size());
      }
}

unsigned vvp_wire_vec8::ମୂଲ୍ୟ_size() ଅଚଳ
{
      ଫେରନ୍ତୁ bits8_.size();
}

vvp_scalar_t vvp_wire_vec8::filtered_ମୂଲ୍ୟ(unsigned idx) ଅଚଳ
{
      ଯଦି (test_force_mask(idx))
	    ଫେରନ୍ତୁ force8_.value(idx);
      ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ bits8_.value(idx);
}

vvp_bit4_t vvp_wire_vec8::value(unsigned idx) ଅଚଳ
{
      ଫେରନ୍ତୁ filtered_ମୂଲ୍ୟ(idx).value();
}

vvp_scalar_t vvp_wire_vec8::scalar_ମୂଲ୍ୟ(unsigned idx) ଅଚଳ
{
      ଫେରନ୍ତୁ filtered_ମୂଲ୍ୟ(idx);
}

vvp_vector8_t vvp_wire_vec8::vec8_ମୂଲ୍ୟ() ଅଚଳ
{
      vvp_vector8_t tmp = bits8_;
      ପାଇଁ (unsigned idx = 0 ; idx < bits8_.size() ; idx += 1)
	    tmp.set_bit(idx, filtered_ମୂଲ୍ୟ(idx));
      ଫେରନ୍ତୁ tmp;
}

ଶୂନ୍ୟ vvp_wire_vec8::vec4_ମୂଲ୍ୟ(vvp_vector4_t&val) ଅଚଳ
{
      val = reduce4(vec8_ମୂଲ୍ୟ());
}

vvp_bit4_t vvp_wire_vec8::driven_ମୂଲ୍ୟ(unsigned idx) ଅଚଳ
{
      ଫେରନ୍ତୁ bits8_.value(idx).value();
}

ତାର୍କିକ vvp_wire_vec8::is_forced(unsigned idx) ଅଚଳ
{
      ଫେରନ୍ତୁ test_force_mask(idx);
}

vvp_wire_real::vvp_wire_real()
: bit_(0.0), force_(0.0)
{
}

vvp_net_fil_t::prop_t vvp_wire_real::filter_real(ଦ୍ୱିଗୁଣ&bit)
{
      ଯଦି (bits_equal(bit_, bit) && !needs_init_) ଫେରନ୍ତୁ STOP;
      bit_ = bit;
      needs_init_ = ମିଥ୍ୟା;
      ଫେରନ୍ତୁ filter_mask_(bit, force_);
}

unsigned vvp_wire_real::filter_size() ଅଚଳ
{
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ vvp_wire_real::force_fil_vec4(ଅଚଳ vvp_vector4_t&, ଅଚଳ vvp_vector2_t&)
{
      ପ୍ରତିଜ୍ଞା(0);
}

ଶୂନ୍ୟ vvp_wire_real::force_fil_vec8(ଅଚଳ vvp_vector8_t&, ଅଚଳ vvp_vector2_t&)
{
      ପ୍ରତିଜ୍ଞା(0);
}

ଶୂନ୍ୟ vvp_wire_real::force_fil_real(ଦ୍ୱିଗୁଣ val, ଅଚଳ vvp_vector2_t&mask)
{
      force_mask(mask);
      ଯଦି (mask.value(0))
	    force_ = val;

      run_vpi_callbacks();
}

ଶୂନ୍ୟ vvp_wire_real::release(vvp_net_ptr_t ptr, ତାର୍କିକ net_flag)
{
      vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
      ଯଦି (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_mask(mask);
	    needs_init_ = !bits_equal(force_, bit_);
	    ptr.ptr()->send_real(bit_, 0);
      } ଅନ୍ୟଥା {
	      // Variables keep the current value.
	    ଦ୍ୱିଗୁଣ res =  real_ମୂଲ୍ୟ();
	    release_mask(mask);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
}

ଶୂନ୍ୟ vvp_wire_real::release_pv(vvp_net_ptr_t, unsigned, unsigned, ତାର୍କିକ)
{
      ପ୍ରତିଜ୍ଞା(0);
#ଯଦି 0
	// A real is a single value. If for some reason this part release
	// can happen the following code should work correctly (requires
	// a base of 0 and a width of 1).
      vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
      ପ୍ରତିଜ୍ଞା(base == 0 && wid == 1);

      ଯଦି (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_mask(mask);
	    ptr.ptr()->send_real(bit_, 0);
      } ଅନ୍ୟଥା {
	      // Variables keep the current value.
	    ଦ୍ୱିଗୁଣ res =  real_ମୂଲ୍ୟ();
	    release_mask(mask);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
#endif
}

unsigned vvp_wire_real::ମୂଲ୍ୟ_size() ଅଚଳ
{
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ 1;
}

vvp_bit4_t vvp_wire_real::value(unsigned) ଅଚଳ
{
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ BIT4_X;
}

vvp_scalar_t vvp_wire_real::scalar_ମୂଲ୍ୟ(unsigned) ଅଚଳ
{
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ vvp_scalar_t();
}

ଶୂନ୍ୟ vvp_wire_real::vec4_ମୂଲ୍ୟ(vvp_vector4_t&) ଅଚଳ
{
      ପ୍ରତିଜ୍ଞା(0);
}

ଦ୍ୱିଗୁଣ vvp_wire_real::real_ମୂଲ୍ୟ() ଅଚଳ
{
      ଯଦି (test_force_mask(0))
	    ଫେରନ୍ତୁ force_;
      ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ bit_;
}

#ଯଦି 0
vvp_wire_string::vvp_wire_string()
{
}

unsigned vvp_wire_string::filter_size() ଅଚଳ
{
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ vvp_wire_string::force_fil_vec4(ଅଚଳ vvp_vector4_t&, ଅଚଳ vvp_vector2_t&)
{
      ପ୍ରତିଜ୍ଞା(0);
}
ଶୂନ୍ୟ vvp_wire_string::force_fil_vec8(ଅଚଳ vvp_vector8_t&, ଅଚଳ vvp_vector2_t&)
{
      ପ୍ରତିଜ୍ଞା(0);
}
ଶୂନ୍ୟ vvp_wire_string::force_fil_real(ଦ୍ୱିଗୁଣ, ଅଚଳ vvp_vector2_t&)
{
      ପ୍ରତିଜ୍ଞା(0);
}

ଶୂନ୍ୟ vvp_wire_string::release(vvp_net_ptr_t ptr, ତାର୍କିକ net_flag)
{
      ପ୍ରତିଜ୍ଞା(0);
}

ଶୂନ୍ୟ vvp_wire_string::release_pv(vvp_net_ptr_t, unsigned, unsigned, ତାର୍କିକ)
{
      ପ୍ରତିଜ୍ଞା(0);
}

unsigned vvp_wire_string::ମୂଲ୍ୟ_size() ଅଚଳ
{
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ 1;
}

vvp_bit4_t vvp_wire_string::value(unsigned) ଅଚଳ
{
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ BIT4_X;
}

vvp_scalar_t vvp_wire_string::scalar_ମୂଲ୍ୟ(unsigned) ଅଚଳ
{
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ vvp_scalar_t();
}

ଶୂନ୍ୟ vvp_wire_string::vec4_ମୂଲ୍ୟ(vvp_vector4_t&) ଅଚଳ
{
      ପ୍ରତିଜ୍ଞା(0);
}

ଦ୍ୱିଗୁଣ vvp_wire_string::real_ମୂଲ୍ୟ() ଅଚଳ
{
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ 0.0;
}
#endif
