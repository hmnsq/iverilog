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

using નામાકાશ std;

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
template <વર્ગ T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_mask_(અચલ T&val, અચલ T&force, T&filter, unsigned base)
{
      જો (!test_force_mask_is_zero()) {
	      // Some bits are being forced. Go through the
	      // force_mask_ and force value to see which bits are
	      // propagated and which are kept from the forced
	      // value. Update the filter with the filtered result and
	      // return REPL to indicate that some bits have changed,
	      // or STOP if no bits change.
	    તાર્કિક propagate_flag = force_propagate_;
	    force_propagate_ = ખોટું;
	    પ્રતિજ્ઞા(force_mask_.size() == force.size());
	    પ્રતિજ્ઞા((base+val.size()) <= force_mask_.size());

	    filter = val;
	    માટે (unsigned idx = 0 ; idx < val.size() ; idx += 1) {
		  જો (force_mask_.value(base+idx))
			filter.set_bit(idx, force.value(base+idx));
		  નહીંતર
			propagate_flag = સાચું;
	    }

	    જો (propagate_flag) {
		  run_vpi_callbacks();
		  પાછા REPL;
	    } નહીંતર {
		  પાછા STOP;
	    }

      } નહીંતર {
	    run_vpi_callbacks();
	    પાછા PROP;
      }
}

template <વર્ગ T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_mask_(T&val, T force)
{

      જો (test_force_mask(0)) {
	    val = force;
	    run_vpi_callbacks();
	    પાછા REPL;
      }
      run_vpi_callbacks();
      પાછા PROP;
}

template <વર્ગ T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_આગત_mask(અચલ T&val, અચલ T&force, T&rep) અચલ
{
      જો (test_force_mask_is_zero())
	    પાછા PROP;

      પ્રતિજ્ઞા(force_mask_.size() == force.size());

      rep = val;
      માટે (unsigned idx = 0 ; idx < val.size() ; idx += 1) {
	    જો (force_mask_.value(idx))
		  rep.set_bit(idx, force.value(idx));
      }

      પાછા REPL;
}

vvp_signal_કિંમત::~vvp_signal_કિંમત()
{
}

દ્વિગુણ vvp_signal_કિંમત::real_કિંમત() અચલ
{
      પ્રતિજ્ઞા(0);
      પાછા 0;
}

શૂન્ય vvp_net_t::force_vec4(અચલ vvp_vector4_t&val, અચલ vvp_vector2_t&mask)
{
      પ્રતિજ્ઞા(fil);
      fil->force_fil_vec4(val, mask);
      fun->force_flag(ખોટું);
      vvp_send_vec4(out_, val, 0);
}

શૂન્ય vvp_net_t::force_vec8(અચલ vvp_vector8_t&val, અચલ vvp_vector2_t&mask)
{
      પ્રતિજ્ઞા(fil);
      fil->force_fil_vec8(val, mask);
      fun->force_flag(ખોટું);
      vvp_send_vec8(out_, val);
}

શૂન્ય vvp_net_t::force_real(દ્વિગુણ val, અચલ vvp_vector2_t&mask)
{
      પ્રતિજ્ઞા(fil);
      fil->force_fil_real(val, mask);
      fun->force_flag(ખોટું);
      vvp_send_real(out_, val, 0);
}

/* **** vvp_fun_signal methods **** */

vvp_fun_signal_base::vvp_fun_signal_base()
{
      continuous_assign_active_ = ખોટું;
      needs_init_ = સાચું;
      cassign_link = 0;
      ગણતરી_functors_sig += 1;
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
શૂન્ય vvp_fun_signal4_sa::recv_vec4(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
                                   vvp_context_t)
{
      પસંદગી (ptr.port()) {
	  સ્થિતિ 0: // Normal input (feed from net, or set from process)
	      /* If we don't have a continuous assign mask then just
		 copy the bits, otherwise we need to see if there are
		 any holes in the mask so we can set those bits. */
	    જો (assign_mask_.size() == 0) {
                  જો (needs_init_ || !bits4_.eeq(bit)) {
			પ્રતિજ્ઞા(bit.size() == bits4_.size());
			bits4_ = bit;
			needs_init_ = ખોટું;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    } નહીંતર {
		  તાર્કિક changed = ખોટું;
		  પ્રતિજ્ઞા(bits4_.size() == assign_mask_.size());
		  માટે (unsigned idx = 0 ;  idx < bit.size() ;  idx += 1) {
			જો (idx >= bits4_.size()) તોડો;
			જો (assign_mask_.value(idx)) ચાલુ;
			bits4_.set_bit(idx, bit.value(idx));
			changed = સાચું;
		  }
		  જો (changed) {
			needs_init_ = ખોટું;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    }
	    તોડો;

	  સ્થિતિ 1: // Continuous assign value
	      // Handle the simple case of the linked source being wider
	      // than this signal. Note we don't yet support the case of
	      // the linked source being narrower than this signal, or
	      // the case of an expression being assigned.
	    bits4_ = coerce_to_width(bit, bits4_.size());
	    assign_mask_ = vvp_vector2_t(vvp_vector2_t::FILL1, bits4_.size());
	    ptr.ptr()->send_vec4(bits4_, 0);
	    તોડો;

	  મૂળભૂત:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    પ્રતિજ્ઞા(0);
	    તોડો;
      }
}

શૂન્ય vvp_fun_signal4_sa::recv_vec8(vvp_net_ptr_t ptr, અચલ vvp_vector8_t&bit)
{
      recv_vec4(ptr, reduce4(bit), 0);
}

શૂન્ય vvp_fun_signal4_sa::recv_vec4_pv(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
				      unsigned base, unsigned vwid, vvp_context_t)
{
      પ્રતિજ્ઞા(bits4_.size() == vwid);
      unsigned wid = bit.size();

      પસંદગી (ptr.port()) {
	  સ્થિતિ 0: // Normal input
	    જો (assign_mask_.size() == 0) {
                  માટે (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
			જો (base+idx >= bits4_.size()) તોડો;
			bits4_.set_bit(base+idx, bit.value(idx));
		  }
		  needs_init_ = ખોટું;
		  ptr.ptr()->send_vec4(bits4_,0);
	    } નહીંતર {
		  તાર્કિક changed = ખોટું;
		  પ્રતિજ્ઞા(bits4_.size() == assign_mask_.size());
		  માટે (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
			જો (base+idx >= bits4_.size()) તોડો;
			જો (assign_mask_.value(base+idx)) ચાલુ;
			bits4_.set_bit(base+idx, bit.value(idx));
			changed = સાચું;
		  }
		  જો (changed) {
			needs_init_ = ખોટું;
			ptr.ptr()->send_vec4(bits4_,0);
		  }
	    }
	    તોડો;

	  સ્થિતિ 1: // Continuous assign value
	    જો (assign_mask_.size() == 0)
		  assign_mask_ = vvp_vector2_t(vvp_vector2_t::FILL0, bits4_.size());
	    માટે (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  જો (base+idx >= bits4_.size())
			તોડો;
		  bits4_.set_bit(base+idx, bit.value(idx));
		  assign_mask_.set_bit(base+idx, 1);
	    }
	    ptr.ptr()->send_vec4(bits4_,0);
	    તોડો;

	  મૂળભૂત:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    પ્રતિજ્ઞા(0);
	    તોડો;
      }
}

શૂન્ય vvp_fun_signal4_sa::recv_vec8_pv(vvp_net_ptr_t ptr, અચલ vvp_vector8_t&bit,
				      unsigned base, unsigned vwid)
{
      recv_vec4_pv(ptr, reduce4(bit), base, vwid, 0);
}

શૂન્ય vvp_fun_signal_base::deassign()
{
      continuous_assign_active_ = ખોટું;
      assign_mask_ = vvp_vector2_t();
}

શૂન્ય vvp_fun_signal_base::deassign_pv(unsigned base, unsigned wid)
{
      માટે (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    assign_mask_.set_bit(base+idx, 0);
      }

      જો (assign_mask_.is_zero()) {
	    assign_mask_ = vvp_vector2_t();
      }
}

શૂન્ય automatic_signal_base::release(vvp_net_ptr_t,તાર્કિક)
{
      પ્રતિજ્ઞા(0);
}

શૂન્ય automatic_signal_base::release_pv(vvp_net_ptr_t,unsigned,unsigned,તાર્કિક)
{
      પ્રતિજ્ઞા(0);
}

unsigned automatic_signal_base::filter_size() અચલ
{
      પ્રતિજ્ઞા(0);
      પાછા(0);
}
શૂન્ય automatic_signal_base::force_fil_vec4(અચલ vvp_vector4_t&, અચલ vvp_vector2_t&)
{
      પ્રતિજ્ઞા(0);
}
શૂન્ય automatic_signal_base::force_fil_vec8(અચલ vvp_vector8_t&, અચલ vvp_vector2_t&)
{
      પ્રતિજ્ઞા(0);
}
શૂન્ય automatic_signal_base::force_fil_real(દ્વિગુણ, અચલ vvp_vector2_t&)
{
      પ્રતિજ્ઞા(0);
}
શૂન્ય automatic_signal_base::get_કિંમત(સંરચના t_vpi_કિંમત*)
{
      પ્રતિજ્ઞા(0);
}

અચલ vvp_vector4_t& vvp_fun_signal4_sa::vec4_unfiltered_કિંમત() અચલ
{
      પાછા bits4_;
}

vvp_fun_signal4_aa::vvp_fun_signal4_aa(unsigned wid, vvp_bit4_t init)
{
      context_idx_ = vpip_add_item_to_context(આ, vpip_peek_context_scope());
      size_ = wid;
      init_ = init;
}

vvp_fun_signal4_aa::~vvp_fun_signal4_aa()
{
      પ્રતિજ્ઞા(0);
}

શૂન્ય vvp_fun_signal4_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, નવું vvp_vector4_t(size_, init_));
}

શૂન્ય vvp_fun_signal4_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*bits = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      bits->fill_bits(init_);
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય vvp_fun_signal4_aa::free_instance(vvp_context_t context)
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
શૂન્ય vvp_fun_signal4_aa::recv_vec4(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      પ્રતિજ્ઞા(ptr.port() == 0);
      પ્રતિજ્ઞા(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      જો (!bits4->eeq(bit)) {
            *bits4 = bit;
            ptr.ptr()->send_vec4(*bits4, context);
      }
}

શૂન્ય vvp_fun_signal4_aa::recv_vec4_pv(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
				      unsigned base, unsigned vwid, vvp_context_t context)
{
      પ્રતિજ્ઞા(ptr.port() == 0);
      પ્રતિજ્ઞા(size_ == vwid);
      પ્રતિજ્ઞા(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      unsigned wid = bit.size();
      માટે (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
            જો (base+idx >= bits4->size()) તોડો;
            bits4->set_bit(base+idx, bit.value(idx));
      }
      ptr.ptr()->send_vec4(*bits4, context);
}

unsigned vvp_fun_signal4_aa::કિંમત_size() અચલ
{
      પાછા size_;
}

vvp_bit4_t vvp_fun_signal4_aa::value(unsigned idx) અચલ
{
      અચલ vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      પાછા bits4->value(idx);
}

vvp_scalar_t vvp_fun_signal4_aa::scalar_કિંમત(unsigned idx) અચલ
{
      અચલ vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      પાછા vvp_scalar_t(bits4->value(idx), 6, 6);
}

શૂન્ય vvp_fun_signal4_aa::vec4_કિંમત(vvp_vector4_t&val) અચલ
{
      અચલ vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      val = *bits4;
}

અચલ vvp_vector4_t&vvp_fun_signal4_aa::vec4_unfiltered_કિંમત() અચલ
{
      અચલ vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      પાછા *bits4;
}

શૂન્ય vvp_fun_signal4_aa::operator delete(શૂન્ય*)
{
      પ્રતિજ્ઞા(0);
}

/*
 * Testing for equality, we want a bitwise test instead of an
 * arithmetic test because we want to treat for example -0 different
 * from +0.
 */
તાર્કિક bits_equal(દ્વિગુણ a, દ્વિગુણ b)
{
      પાછા memcmp(&a, &b, કદ a) == 0;
}

vvp_fun_signal_real_sa::vvp_fun_signal_real_sa()
{
      bits_ = 0.0;
}

દ્વિગુણ vvp_fun_signal_real_sa::real_unfiltered_કિંમત() અચલ
{
      પાછા bits_;
}

શૂન્ય vvp_fun_signal_real_sa::recv_real(vvp_net_ptr_t ptr, દ્વિગુણ bit,
                                       vvp_context_t)
{
      પસંદગી (ptr.port()) {
	  સ્થિતિ 0:
	    જો (!continuous_assign_active_) {
                  જો (needs_init_ || !bits_equal(bits_, bit)) {
			bits_ = bit;
			needs_init_ = ખોટું;
			ptr.ptr()->send_real(bit, 0);
		  }
	    }
	    તોડો;

	  સ્થિતિ 1: // Continuous assign value
	    continuous_assign_active_ = સાચું;
	    bits_ = bit;
	    ptr.ptr()->send_real(bit, 0);
	    તોડો;

	  મૂળભૂત:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    પ્રતિજ્ઞા(0);
	    તોડો;
      }
}

vvp_fun_signal_real_aa::vvp_fun_signal_real_aa()
{
      context_idx_ = vpip_add_item_to_context(આ, vpip_peek_context_scope());
}

vvp_fun_signal_real_aa::~vvp_fun_signal_real_aa()
{
      પ્રતિજ્ઞા(0);
}

શૂન્ય vvp_fun_signal_real_aa::alloc_instance(vvp_context_t context)
{
      દ્વિગુણ*bits = નવું દ્વિગુણ;
      vvp_set_context_item(context, context_idx_, bits);

      *bits = 0.0;
}

શૂન્ય vvp_fun_signal_real_aa::reset_instance(vvp_context_t context)
{
      દ્વિગુણ*bits = static_cast<દ્વિગુણ*>
            (vvp_get_context_item(context, context_idx_));

      *bits = 0.0;
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય vvp_fun_signal_real_aa::free_instance(vvp_context_t context)
{
      દ્વિગુણ*bits = static_cast<દ્વિગુણ*>
            (vvp_get_context_item(context, context_idx_));
      delete bits;
}
#endif

દ્વિગુણ vvp_fun_signal_real_aa::real_unfiltered_કિંમત() અચલ
{
      અચલ દ્વિગુણ*bits = static_cast<દ્વિગુણ*>
            (vthread_get_rd_context_item(context_idx_));

      પાછા *bits;
}

દ્વિગુણ vvp_fun_signal_real_aa::real_કિંમત() અચલ
{
      પાછા real_unfiltered_કિંમત();
}

શૂન્ય vvp_fun_signal_real_aa::recv_real(vvp_net_ptr_t ptr, દ્વિગુણ bit,
                                       vvp_context_t context)
{
      પ્રતિજ્ઞા(ptr.port() == 0);
      પ્રતિજ્ઞા(context);

      દ્વિગુણ*bits = static_cast<દ્વિગુણ*>
            (vvp_get_context_item(context, context_idx_));

      જો (!bits_equal(*bits,bit)) {
            *bits = bit;
            ptr.ptr()->send_real(bit, context);
      }
}

unsigned vvp_fun_signal_real_aa::કિંમત_size() અચલ
{
      પ્રતિજ્ઞા(0);
      પાછા 1;
}

vvp_bit4_t vvp_fun_signal_real_aa::value(unsigned) અચલ
{
      પ્રતિજ્ઞા(0);
      પાછા BIT4_X;
}

vvp_scalar_t vvp_fun_signal_real_aa::scalar_કિંમત(unsigned) અચલ
{
      પ્રતિજ્ઞા(0);
      પાછા vvp_scalar_t();
}

શૂન્ય vvp_fun_signal_real_aa::vec4_કિંમત(vvp_vector4_t&) અચલ
{
      પ્રતિજ્ઞા(0);
}

શૂન્ય* vvp_fun_signal_real_aa::operator નવું(std::size_t size)
{
      પાછા vvp_net_fun_t::heap_.alloc(size);
}

શૂન્ય vvp_fun_signal_real_aa::operator delete(શૂન્ય*)
{
      પ્રતિજ્ઞા(0);
}


vvp_fun_signal_string_sa::vvp_fun_signal_string_sa()
{
}

શૂન્ય vvp_fun_signal_string_sa::recv_string(vvp_net_ptr_t ptr, અચલ std::string&bit,
					   vvp_context_t)
{
      પ્રતિજ્ઞા(ptr.port() == 0);

      જો (needs_init_ || કિંમત_2 != bit) {
	    કિંમત_2 = bit;
	    needs_init_ = ખોટું;

	    ptr.ptr()->send_string(bit, 0);
      }
}

અચલ string& vvp_fun_signal_string_sa::get_string() અચલ
{
      પાછા કિંમત_2;
}

vvp_fun_signal_string_aa::vvp_fun_signal_string_aa()
{
      context_idx_ = vpip_add_item_to_context(આ, vpip_peek_context_scope());
}

vvp_fun_signal_string_aa::~vvp_fun_signal_string_aa()
{
      પ્રતિજ્ઞા(0);
}

શૂન્ય vvp_fun_signal_string_aa::alloc_instance(vvp_context_t context)
{
      string*bits = નવું std::string;
      vvp_set_context_item(context, context_idx_, bits);
      *bits = "";
}

શૂન્ય vvp_fun_signal_string_aa::reset_instance(vvp_context_t context)
{
      string*bits = static_cast<std::string*>
	    (vvp_get_context_item(context, context_idx_));
      *bits = "";
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય vvp_fun_signal_string_aa::free_instance(vvp_context_t context)
{
      string*bits = static_cast<std::string*>
            (vvp_get_context_item(context, context_idx_));
      delete bits;
}
#endif

શૂન્ય vvp_fun_signal_string_aa::recv_string(vvp_net_ptr_t ptr, અચલ std::string&bit, vvp_context_t context)
{
      પ્રતિજ્ઞા(ptr.port() == 0);
      પ્રતિજ્ઞા(context);

      string*bits = static_cast<std::string*>
	    (vvp_get_context_item(context, context_idx_));

      જો (*bits != bit) {
	    *bits = bit;
	    ptr.ptr()->send_string(bit, context);
      }
}

unsigned vvp_fun_signal_string_aa::કિંમત_size() અચલ
{
      પ્રતિજ્ઞા(0);
      પાછા 1;
}

vvp_bit4_t vvp_fun_signal_string_aa::value(unsigned) અચલ
{
      પ્રતિજ્ઞા(0);
      પાછા BIT4_X;
}

vvp_scalar_t vvp_fun_signal_string_aa::scalar_કિંમત(unsigned) અચલ
{
      પ્રતિજ્ઞા(0);
      પાછા vvp_scalar_t();
}

શૂન્ય vvp_fun_signal_string_aa::vec4_કિંમત(vvp_vector4_t&) અચલ
{
      પ્રતિજ્ઞા(0);
}

દ્વિગુણ vvp_fun_signal_string_aa::real_કિંમત() અચલ
{
      પ્રતિજ્ઞા(0);
      પાછા 0.0;
}

અચલ std::string& vvp_fun_signal_string_aa::get_string() અચલ
{
      અચલ string*bits = static_cast<std::string*>
            (vthread_get_rd_context_item(context_idx_));

      પાછા *bits;
}

શૂન્ય* vvp_fun_signal_string_aa::operator નવું(std::size_t size)
{
      પાછા vvp_net_fun_t::heap_.alloc(size);
}

શૂન્ય vvp_fun_signal_string_aa::operator delete(શૂન્ય*)
{
      પ્રતિજ્ઞા(0);
}

  /* OBJECT signals */

vvp_fun_signal_object_sa::vvp_fun_signal_object_sa(unsigned size)
: vvp_fun_signal_object(size)
{
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય vvp_fun_signal_object_aa::free_instance(vvp_context_t context)
{
      vvp_object_t*bits = static_cast<vvp_object_t*>
            (vvp_get_context_item(context, context_idx_));
      delete bits;
}
#endif

શૂન્ય vvp_fun_signal_object_sa::recv_object(vvp_net_ptr_t ptr, vvp_object_t bit,
					   vvp_context_t)
{
      પ્રતિજ્ઞા(ptr.port() == 0);

      જો (needs_init_ || કિંમત_2 != bit) {
	    કિંમત_2 = bit;
	    needs_init_ = ખોટું;

	    ptr.ptr()->send_object(bit, 0);
      }
}

vvp_object_t vvp_fun_signal_object_sa::get_object() અચલ
{
      પાછા કિંમત_2;
}

vvp_fun_signal_object_aa::vvp_fun_signal_object_aa(unsigned size)
: vvp_fun_signal_object(size)
{
      context_idx_ = vpip_add_item_to_context(આ, vpip_peek_context_scope());
}

vvp_fun_signal_object_aa::~vvp_fun_signal_object_aa()
{
      પ્રતિજ્ઞા(0);
}

શૂન્ય vvp_fun_signal_object_aa::alloc_instance(vvp_context_t context)
{
      vvp_object_t*bits = નવું vvp_object_t;
      vvp_set_context_item(context, context_idx_, bits);
      bits->reset();
}

શૂન્ય vvp_fun_signal_object_aa::reset_instance(vvp_context_t context)
{
      vvp_object_t*bits = static_cast<vvp_object_t*>
	    (vvp_get_context_item(context, context_idx_));
      bits->reset();
}

vvp_object_t vvp_fun_signal_object_aa::get_object() અચલ
{
      અચલ vvp_object_t*bits = static_cast<vvp_object_t*>
	    (vthread_get_rd_context_item(context_idx_));
      પાછા *bits;
}

શૂન્ય vvp_fun_signal_object_aa::recv_object(vvp_net_ptr_t ptr, vvp_object_t bit,
					   vvp_context_t context)
{
      પ્રતિજ્ઞા(ptr.port() == 0);
      પ્રતિજ્ઞા(context);

      vvp_object_t*bits = static_cast<vvp_object_t*>
	    (vvp_get_context_item(context, context_idx_));

      જો (*bits != bit) {
	    *bits = bit;
	    ptr.ptr()->send_object(bit, context);
      }
}

unsigned vvp_fun_signal_object_aa::કિંમત_size() અચલ
{
      પાછા 1;
}

vvp_bit4_t vvp_fun_signal_object_aa::value(unsigned) અચલ
{
      પ્રતિજ્ઞા(0);
      પાછા BIT4_X;
}

vvp_scalar_t vvp_fun_signal_object_aa::scalar_કિંમત(unsigned) અચલ
{
      પ્રતિજ્ઞા(0);
      પાછા vvp_scalar_t();
}

શૂન્ય vvp_fun_signal_object_aa::vec4_કિંમત(vvp_vector4_t&) અચલ
{
      પ્રતિજ્ઞા(0);
}

શૂન્ય* vvp_fun_signal_object_aa::operator નવું(std::size_t size)
{
      પાછા vvp_net_fun_t::heap_.alloc(size);
}

શૂન્ય vvp_fun_signal_object_aa::operator delete(શૂન્ય*)
{
      પ્રતિજ્ઞા(0);
}

  /* **** */

vvp_fun_force::vvp_fun_force()
{
}

vvp_fun_force::~vvp_fun_force()
{
}

શૂન્ય vvp_fun_force::recv_vec4(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
			      vvp_context_t)
{
      પ્રતિજ્ઞા(ptr.port() == 0);
      vvp_net_t*net = ptr.ptr();

      vvp_net_t*dst = net->port[3].ptr();
      પ્રતિજ્ઞા(dst->fil);

      dst->force_vec4(coerce_to_width(bit, dst->fil->filter_size()), vvp_vector2_t(vvp_vector2_t::FILL1, dst->fil->filter_size()));
}

શૂન્ય vvp_fun_force::recv_real(vvp_net_ptr_t ptr, દ્વિગુણ bit, vvp_context_t)
{
      પ્રતિજ્ઞા(ptr.port() == 0);
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

vvp_bit4_t vvp_wire_base::driven_કિંમત(unsigned) અચલ
{
      પ્રતિજ્ઞા(0);
      પાછા BIT4_X;
}

તાર્કિક vvp_wire_base::is_forced(unsigned) અચલ
{
      પ્રતિજ્ઞા(0);
      પાછા ખોટું;
}

vvp_wire_vec4::vvp_wire_vec4(unsigned wid, vvp_bit4_t init)
: bits4_(wid, init)
{
      needs_init_ = સાચું;
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec4(અચલ vvp_vector4_t&bit, vvp_vector4_t&rep,
						 unsigned base, unsigned vwid)
{
	// Special case! the input bit is 0 wid. Interpret this as a
	// vector of BIT4_X to match the width of the bits4_ vector.
	// FIXME! This is a hack to work around some buggy gate
	// implementations! This should be removed!
      જો (base==0 && vwid==0) {
	    vvp_vector4_t tmp (bits4_.size(), BIT4_X);
	    જો (bits4_ .eeq(tmp) && !needs_init_) પાછા STOP;
	    bits4_ = tmp;
	    needs_init_ = ખોટું;
	    પાછા filter_mask_(tmp, force4_, rep, 0);
      }

      જો (vwid != bits4_.size()) {
	    cerr << "Internal error: Input vector expected width="
		 << bits4_.size() << ", got "
		 << "bit=" << bit << ", base=" << base << ", vwid=" << vwid
		 << endl;
      }
      પ્રતિજ્ઞા(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      જો (base==0 && bit.size()==vwid) {
	    જો (bits4_ .eeq( bit ) && !needs_init_) પાછા STOP;
	    bits4_ = bit;
      } નહીંતર {
	    તાર્કિક rc = bits4_.set_vec(base, bit);
	    જો (rc == ખોટું && !needs_init_) પાછા STOP;
      }

      needs_init_ = ખોટું;
      પાછા filter_mask_(bit, force4_, rep, base);
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec8(અચલ vvp_vector8_t&bit,
                                                 vvp_vector8_t&rep,
                                                 unsigned base,
                                                 unsigned vwid)
{
      પ્રતિજ્ઞા(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      vvp_vector4_t bit4 (reduce4(bit));
      જો (base==0 && bit4.size()==vwid) {
	    જો (bits4_ .eeq( bit4 ) && !needs_init_) પાછા STOP;
	    bits4_ = bit4;
      } નહીંતર {
	    તાર્કિક rc = bits4_.set_vec(base, bit4);
	    જો (rc == ખોટું && !needs_init_) પાછા STOP;
      }

      needs_init_ = ખોટું;
      પાછા filter_mask_(bit, vvp_vector8_t(force4_,6,6), rep, base);
}

unsigned vvp_wire_vec4::filter_size() અચલ
{
      પાછા bits4_.size();
}

શૂન્ય vvp_wire_vec4::force_fil_vec4(અચલ vvp_vector4_t&val, અચલ vvp_vector2_t&mask)
{
      force_mask(mask);

      જો (force4_.size() == 0) {
	    force4_ = val;
      } નહીંતર {
	    માટે (unsigned idx = 0; idx < mask.size() ; idx += 1) {
		  જો (mask.value(idx) == 0)
			ચાલુ;

		  force4_.set_bit(idx, val.value(idx));
	    }
      }
      run_vpi_callbacks();
}

શૂન્ય vvp_wire_vec4::force_fil_vec8(અચલ vvp_vector8_t&, અચલ vvp_vector2_t&)
{
      પ્રતિજ્ઞા(0);
}

શૂન્ય vvp_wire_vec4::force_fil_real(દ્વિગુણ, અચલ vvp_vector2_t&)
{
      પ્રતિજ્ઞા(0);
}

શૂન્ય vvp_wire_vec4::release(vvp_net_ptr_t ptr, તાર્કિક net_flag)
{
      vvp_vector2_t mask (vvp_vector2_t::FILL1, bits4_.size());
      જો (net_flag) {
	      // Wires revert to their unforced value after release.
            release_mask(mask);
	    needs_init_ = ! force4_ .eeq(bits4_);
	    ptr.ptr()->send_vec4(bits4_, 0);
	    run_vpi_callbacks();
      } નહીંતર {
	      // Variables keep the current value.
	    vvp_vector4_t res (bits4_.size());
	    માટે (unsigned idx=0; idx<bits4_.size(); idx += 1)
		  res.set_bit(idx,value(idx));
            release_mask(mask);
	    ptr.ptr()->fun->recv_vec4(ptr, res, 0);
      }
}

શૂન્ય vvp_wire_vec4::release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, તાર્કિક net_flag)
{
      પ્રતિજ્ઞા(bits4_.size() >= base + wid);

      vvp_vector2_t mask (vvp_vector2_t::FILL0, bits4_.size());
      માટે (unsigned idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      જો (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_mask(mask);
	    needs_init_ = ! force4_.subvalue(base,wid) .eeq(bits4_.subvalue(base,wid));
	    ptr.ptr()->send_vec4_pv(bits4_.subvalue(base,wid),
				    base, bits4_.size(), 0);
	    run_vpi_callbacks();
      } નહીંતર {
	      // Variables keep the current value.
	    vvp_vector4_t res (wid);
	    માટે (unsigned idx=0; idx<wid; idx += 1)
		  res.set_bit(idx,value(base+idx));
	    release_mask(mask);
	    ptr.ptr()->fun->recv_vec4_pv(ptr, res, base, bits4_.size(), 0);
      }
}

unsigned vvp_wire_vec4::કિંમત_size() અચલ
{
      પાછા bits4_.size();
}

vvp_bit4_t vvp_wire_vec4::filtered_કિંમત(unsigned idx) અચલ
{
      જો (test_force_mask(idx))
	    પાછા force4_.value(idx);
      નહીંતર
	    પાછા bits4_.value(idx);
}

vvp_bit4_t vvp_wire_vec4::value(unsigned idx) અચલ
{
      પાછા filtered_કિંમત(idx);
}

vvp_scalar_t vvp_wire_vec4::scalar_કિંમત(unsigned idx) અચલ
{
      પાછા vvp_scalar_t(value(idx),6,6);
}

શૂન્ય vvp_wire_vec4::vec4_કિંમત(vvp_vector4_t&val) અચલ
{
      val = bits4_;
      જો (test_force_mask_is_zero())
	    પાછા;

      માટે (unsigned idx = 0 ; idx < bits4_.size() ; idx += 1)
	    val.set_bit(idx, filtered_કિંમત(idx));
}

vvp_bit4_t vvp_wire_vec4::driven_કિંમત(unsigned idx) અચલ
{
      પાછા bits4_.value(idx);
}

તાર્કિક vvp_wire_vec4::is_forced(unsigned idx) અચલ
{
      પાછા test_force_mask(idx);
}

vvp_wire_vec8::vvp_wire_vec8(unsigned wid)
: bits8_(wid)
{
      needs_init_ = સાચું;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec4(અચલ vvp_vector4_t&bit,
                                                 vvp_vector4_t&rep,
                                                 unsigned base,
                                                 unsigned vwid)
{
	// For now there is no support for a non-zero base.
      પ્રતિજ્ઞા(0 == base);
      પ્રતિજ્ઞા(bits8_.size() == vwid);
      પ્રતિજ્ઞા(bits8_.size() == bit.size());
	// QUESTION: Is it really correct to propagate a vec4 if this
	// is a vec8 node? In fact, it is really possible for a vec4
	// value to get through to a vec8 filter?
      vvp_vector8_t rep8;
      prop_t rc = filter_vec8(vvp_vector8_t(bit,6,6), rep8, 0, vwid);
      જો (rc == REPL)
	    rep = reduce4(rep8);

      needs_init_ = ખોટું;
      પાછા rc;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec8(અચલ vvp_vector8_t&bit, vvp_vector8_t&rep, unsigned base, unsigned vwid)
{
      પ્રતિજ્ઞા(vwid == bits8_.size());
	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      જો (base==0 && bit.size()==vwid) {
	    જો (bits8_.eeq(bit) && !needs_init_) પાછા STOP;
	    bits8_ = bit;
      } નહીંતર {
	    જો (bits8_.subvalue(base, bit.size()).eeq(bit) &&
		!needs_init_) {
		  પાછા STOP;
	    }
	    જો (bits8_.size() == 0)
		  bits8_ = vvp_vector8_t(vwid);
	    પ્રતિજ્ઞા(bits8_.size() == vwid);
	    bits8_.set_vec(base, bit);
      }
      needs_init_ = ખોટું;
      પાછા filter_mask_(bit, force8_, rep, base);
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_આગત_vec8(અચલ vvp_vector8_t&bit, vvp_vector8_t&rep) અચલ
{
      પાછા filter_આગત_mask(bit, force8_, rep);
}

unsigned vvp_wire_vec8::filter_size() અચલ
{
      પાછા bits8_.size();
}

શૂન્ય vvp_wire_vec8::force_fil_vec4(અચલ vvp_vector4_t&val, અચલ vvp_vector2_t&mask)
{
      force_fil_vec8(vvp_vector8_t(val,6,6), mask);
}

શૂન્ય vvp_wire_vec8::force_fil_vec8(અચલ vvp_vector8_t&val, અચલ vvp_vector2_t&mask)
{
      force_mask(mask);

      જો (force8_.size() == 0) {
	    force8_ = val;
      } નહીંતર {
	    માટે (unsigned idx = 0; idx < mask.size() ; idx += 1) {
		  જો (mask.value(idx) == 0)
			ચાલુ;

		  force8_.set_bit(idx, val.value(idx));
	    }
      }
      run_vpi_callbacks();
}

શૂન્ય vvp_wire_vec8::force_fil_real(દ્વિગુણ, અચલ vvp_vector2_t&)
{
      પ્રતિજ્ઞા(0);
}

શૂન્ય vvp_wire_vec8::release(vvp_net_ptr_t ptr, તાર્કિક net_flag)
{
	// Wires revert to their unforced value after release.
      vvp_vector2_t mask (vvp_vector2_t::FILL1, bits8_.size());
      release_mask(mask);
      જો (net_flag) {
	    needs_init_ = !force8_ .eeq(bits8_);
	    ptr.ptr()->send_vec8(bits8_);
      } નહીંતર {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    પ્રતિજ્ઞા(0);
//	    ptr.ptr()->fun->recv_vec8(ptr, force8_);
      }
}

શૂન્ય vvp_wire_vec8::release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, તાર્કિક net_flag)
{
      પ્રતિજ્ઞા(bits8_.size() >= base + wid);

      vvp_vector2_t mask (vvp_vector2_t::FILL0, bits8_.size());
      માટે (unsigned idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      release_mask(mask);

      જો (net_flag) {
	    needs_init_ = !force8_.subvalue(base,wid) .eeq((bits8_.subvalue(base,wid)));
	    ptr.ptr()->send_vec8_pv(bits8_.subvalue(base,wid),
				    base, bits8_.size());
	    run_vpi_callbacks();
      } નહીંતર {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    પ્રતિજ્ઞા(0);
//	    ptr.ptr()->fun->recv_vec8_pv(ptr, force8_.subvalue(base,wid),
//					 base, force8_.size());
      }
}

unsigned vvp_wire_vec8::કિંમત_size() અચલ
{
      પાછા bits8_.size();
}

vvp_scalar_t vvp_wire_vec8::filtered_કિંમત(unsigned idx) અચલ
{
      જો (test_force_mask(idx))
	    પાછા force8_.value(idx);
      નહીંતર
	    પાછા bits8_.value(idx);
}

vvp_bit4_t vvp_wire_vec8::value(unsigned idx) અચલ
{
      પાછા filtered_કિંમત(idx).value();
}

vvp_scalar_t vvp_wire_vec8::scalar_કિંમત(unsigned idx) અચલ
{
      પાછા filtered_કિંમત(idx);
}

vvp_vector8_t vvp_wire_vec8::vec8_કિંમત() અચલ
{
      vvp_vector8_t tmp = bits8_;
      માટે (unsigned idx = 0 ; idx < bits8_.size() ; idx += 1)
	    tmp.set_bit(idx, filtered_કિંમત(idx));
      પાછા tmp;
}

શૂન્ય vvp_wire_vec8::vec4_કિંમત(vvp_vector4_t&val) અચલ
{
      val = reduce4(vec8_કિંમત());
}

vvp_bit4_t vvp_wire_vec8::driven_કિંમત(unsigned idx) અચલ
{
      પાછા bits8_.value(idx).value();
}

તાર્કિક vvp_wire_vec8::is_forced(unsigned idx) અચલ
{
      પાછા test_force_mask(idx);
}

vvp_wire_real::vvp_wire_real()
: bit_(0.0), force_(0.0)
{
}

vvp_net_fil_t::prop_t vvp_wire_real::filter_real(દ્વિગુણ&bit)
{
      જો (bits_equal(bit_, bit) && !needs_init_) પાછા STOP;
      bit_ = bit;
      needs_init_ = ખોટું;
      પાછા filter_mask_(bit, force_);
}

unsigned vvp_wire_real::filter_size() અચલ
{
      પ્રતિજ્ઞા(0);
      પાછા 0;
}

શૂન્ય vvp_wire_real::force_fil_vec4(અચલ vvp_vector4_t&, અચલ vvp_vector2_t&)
{
      પ્રતિજ્ઞા(0);
}

શૂન્ય vvp_wire_real::force_fil_vec8(અચલ vvp_vector8_t&, અચલ vvp_vector2_t&)
{
      પ્રતિજ્ઞા(0);
}

શૂન્ય vvp_wire_real::force_fil_real(દ્વિગુણ val, અચલ vvp_vector2_t&mask)
{
      force_mask(mask);
      જો (mask.value(0))
	    force_ = val;

      run_vpi_callbacks();
}

શૂન્ય vvp_wire_real::release(vvp_net_ptr_t ptr, તાર્કિક net_flag)
{
      vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
      જો (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_mask(mask);
	    needs_init_ = !bits_equal(force_, bit_);
	    ptr.ptr()->send_real(bit_, 0);
      } નહીંતર {
	      // Variables keep the current value.
	    દ્વિગુણ res =  real_કિંમત();
	    release_mask(mask);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
}

શૂન્ય vvp_wire_real::release_pv(vvp_net_ptr_t, unsigned, unsigned, તાર્કિક)
{
      પ્રતિજ્ઞા(0);
#જો 0
	// A real is a single value. If for some reason this part release
	// can happen the following code should work correctly (requires
	// a base of 0 and a width of 1).
      vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
      પ્રતિજ્ઞા(base == 0 && wid == 1);

      જો (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_mask(mask);
	    ptr.ptr()->send_real(bit_, 0);
      } નહીંતર {
	      // Variables keep the current value.
	    દ્વિગુણ res =  real_કિંમત();
	    release_mask(mask);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
#endif
}

unsigned vvp_wire_real::કિંમત_size() અચલ
{
      પ્રતિજ્ઞા(0);
      પાછા 1;
}

vvp_bit4_t vvp_wire_real::value(unsigned) અચલ
{
      પ્રતિજ્ઞા(0);
      પાછા BIT4_X;
}

vvp_scalar_t vvp_wire_real::scalar_કિંમત(unsigned) અચલ
{
      પ્રતિજ્ઞા(0);
      પાછા vvp_scalar_t();
}

શૂન્ય vvp_wire_real::vec4_કિંમત(vvp_vector4_t&) અચલ
{
      પ્રતિજ્ઞા(0);
}

દ્વિગુણ vvp_wire_real::real_કિંમત() અચલ
{
      જો (test_force_mask(0))
	    પાછા force_;
      નહીંતર
	    પાછા bit_;
}

#જો 0
vvp_wire_string::vvp_wire_string()
{
}

unsigned vvp_wire_string::filter_size() અચલ
{
      પ્રતિજ્ઞા(0);
      પાછા 0;
}

શૂન્ય vvp_wire_string::force_fil_vec4(અચલ vvp_vector4_t&, અચલ vvp_vector2_t&)
{
      પ્રતિજ્ઞા(0);
}
શૂન્ય vvp_wire_string::force_fil_vec8(અચલ vvp_vector8_t&, અચલ vvp_vector2_t&)
{
      પ્રતિજ્ઞા(0);
}
શૂન્ય vvp_wire_string::force_fil_real(દ્વિગુણ, અચલ vvp_vector2_t&)
{
      પ્રતિજ્ઞા(0);
}

શૂન્ય vvp_wire_string::release(vvp_net_ptr_t ptr, તાર્કિક net_flag)
{
      પ્રતિજ્ઞા(0);
}

શૂન્ય vvp_wire_string::release_pv(vvp_net_ptr_t, unsigned, unsigned, તાર્કિક)
{
      પ્રતિજ્ઞા(0);
}

unsigned vvp_wire_string::કિંમત_size() અચલ
{
      પ્રતિજ્ઞા(0);
      પાછા 1;
}

vvp_bit4_t vvp_wire_string::value(unsigned) અચલ
{
      પ્રતિજ્ઞા(0);
      પાછા BIT4_X;
}

vvp_scalar_t vvp_wire_string::scalar_કિંમત(unsigned) અચલ
{
      પ્રતિજ્ઞા(0);
      પાછા vvp_scalar_t();
}

શૂન્ય vvp_wire_string::vec4_કિંમત(vvp_vector4_t&) અચલ
{
      પ્રતિજ્ઞા(0);
}

દ્વિગુણ vvp_wire_string::real_કિંમત() અચલ
{
      પ્રતિજ્ઞા(0);
      પાછા 0.0;
}
#endif
