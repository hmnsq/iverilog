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
#अगर_घोषित CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
# include  <map>
#पूर्ण_अगर

# include  <iostream>

using namespace std;

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
ढाँचा <श्रेणी T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_mask_(स्थिर T&val, स्थिर T&force, T&filter, अचिन्हित base)
{
      अगर (!test_force_mask_is_zero()) {
	      // Some bits are being forced. Go through the
	      // force_mask_ and force value to see which bits are
	      // propagated and which are kept from the forced
	      // value. Update the filter with the filtered result and
	      // return REPL to indicate that some bits have changed,
	      // or STOP if no bits change.
	    bool propagate_flag = force_propagate_;
	    force_propagate_ = false;
	    निश्चित(force_mask_.size() == force.size());
	    निश्चित((base+val.size()) <= force_mask_.size());

	    filter = val;
	    क्रम (अचिन्हित idx = 0 ; idx < val.size() ; idx += 1) {
		  अगर (force_mask_.value(base+idx))
			filter.set_bit(idx, force.value(base+idx));
		  अन्यथा
			propagate_flag = true;
	    }

	    अगर (propagate_flag) {
		  run_vpi_callbacks();
		  वापस REPL;
	    } अन्यथा {
		  वापस STOP;
	    }

      } अन्यथा {
	    run_vpi_callbacks();
	    वापस PROP;
      }
}

ढाँचा <श्रेणी T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_mask_(T&val, T force)
{

      अगर (test_force_mask(0)) {
	    val = force;
	    run_vpi_callbacks();
	    वापस REPL;
      }
      run_vpi_callbacks();
      वापस PROP;
}

ढाँचा <श्रेणी T> vvp_net_fil_t::prop_t vvp_net_fil_t::filter_निवेश_mask(स्थिर T&val, स्थिर T&force, T&rep) स्थिर
{
      अगर (test_force_mask_is_zero())
	    वापस PROP;

      निश्चित(force_mask_.size() == force.size());

      rep = val;
      क्रम (अचिन्हित idx = 0 ; idx < val.size() ; idx += 1) {
	    अगर (force_mask_.value(idx))
		  rep.set_bit(idx, force.value(idx));
      }

      वापस REPL;
}

vvp_signal_मूल्य::~vvp_signal_मूल्य()
{
}

द्विगुन vvp_signal_मूल्य::real_मूल्य() स्थिर
{
      निश्चित(0);
      वापस 0;
}

व्योम vvp_net_t::force_vec4(स्थिर vvp_vector4_t&val, स्थिर vvp_vector2_t&mask)
{
      निश्चित(fil);
      fil->force_fil_vec4(val, mask);
      fun->force_flag(false);
      vvp_send_vec4(out_, val, 0);
}

व्योम vvp_net_t::force_vec8(स्थिर vvp_vector8_t&val, स्थिर vvp_vector2_t&mask)
{
      निश्चित(fil);
      fil->force_fil_vec8(val, mask);
      fun->force_flag(false);
      vvp_send_vec8(out_, val);
}

व्योम vvp_net_t::force_real(द्विगुन val, स्थिर vvp_vector2_t&mask)
{
      निश्चित(fil);
      fil->force_fil_real(val, mask);
      fun->force_flag(false);
      vvp_send_real(out_, val, 0);
}

/* **** vvp_fun_signal methods **** */

vvp_fun_signal_base::vvp_fun_signal_base()
{
      continuous_assign_active_ = false;
      needs_init_ = true;
      cassign_link = 0;
      गिनती_functors_sig += 1;
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
व्योम vvp_fun_signal4_sa::recv_vec4(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
                                   vvp_context_t)
{
      चयन (ptr.port()) {
	  हाल 0: // Normal input (feed from net, or set from process)
	      /* If we don't have a continuous assign mask then just
		 copy the bits, otherwise we need to see if there are
		 any holes in the mask so we can set those bits. */
	    अगर (assign_mask_.size() == 0) {
                  अगर (needs_init_ || !bits4_.eeq(bit)) {
			निश्चित(bit.size() == bits4_.size());
			bits4_ = bit;
			needs_init_ = false;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    } अन्यथा {
		  bool changed = false;
		  निश्चित(bits4_.size() == assign_mask_.size());
		  क्रम (अचिन्हित idx = 0 ;  idx < bit.size() ;  idx += 1) {
			अगर (idx >= bits4_.size()) अवरोध;
			अगर (assign_mask_.value(idx)) जारी;
			bits4_.set_bit(idx, bit.value(idx));
			changed = true;
		  }
		  अगर (changed) {
			needs_init_ = false;
			ptr.ptr()->send_vec4(bits4_, 0);
		  }
	    }
	    अवरोध;

	  हाल 1: // Continuous assign value
	      // Handle the simple case of the linked source being wider
	      // than this signal. Note we don't yet support the case of
	      // the linked source being narrower than this signal, or
	      // the case of an expression being assigned.
	    bits4_ = coerce_to_width(bit, bits4_.size());
	    assign_mask_ = vvp_vector2_t(vvp_vector2_t::FILL1, bits4_.size());
	    ptr.ptr()->send_vec4(bits4_, 0);
	    अवरोध;

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "Unsupported port type %u.\n", ptr.port());
	    निश्चित(0);
	    अवरोध;
      }
}

व्योम vvp_fun_signal4_sa::recv_vec8(vvp_net_ptr_t ptr, स्थिर vvp_vector8_t&bit)
{
      recv_vec4(ptr, reduce4(bit), 0);
}

व्योम vvp_fun_signal4_sa::recv_vec4_pv(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
				      अचिन्हित base, अचिन्हित vwid, vvp_context_t)
{
      निश्चित(bits4_.size() == vwid);
      अचिन्हित wid = bit.size();

      चयन (ptr.port()) {
	  हाल 0: // Normal input
	    अगर (assign_mask_.size() == 0) {
                  क्रम (अचिन्हित idx = 0 ;  idx < wid ;  idx += 1) {
			अगर (base+idx >= bits4_.size()) अवरोध;
			bits4_.set_bit(base+idx, bit.value(idx));
		  }
		  needs_init_ = false;
		  ptr.ptr()->send_vec4(bits4_,0);
	    } अन्यथा {
		  bool changed = false;
		  निश्चित(bits4_.size() == assign_mask_.size());
		  क्रम (अचिन्हित idx = 0 ;  idx < wid ;  idx += 1) {
			अगर (base+idx >= bits4_.size()) अवरोध;
			अगर (assign_mask_.value(base+idx)) जारी;
			bits4_.set_bit(base+idx, bit.value(idx));
			changed = true;
		  }
		  अगर (changed) {
			needs_init_ = false;
			ptr.ptr()->send_vec4(bits4_,0);
		  }
	    }
	    अवरोध;

	  हाल 1: // Continuous assign value
	    अगर (assign_mask_.size() == 0)
		  assign_mask_ = vvp_vector2_t(vvp_vector2_t::FILL0, bits4_.size());
	    क्रम (अचिन्हित idx = 0 ;  idx < wid ;  idx += 1) {
		  अगर (base+idx >= bits4_.size())
			अवरोध;
		  bits4_.set_bit(base+idx, bit.value(idx));
		  assign_mask_.set_bit(base+idx, 1);
	    }
	    ptr.ptr()->send_vec4(bits4_,0);
	    अवरोध;

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "Unsupported port type %u.\n", ptr.port());
	    निश्चित(0);
	    अवरोध;
      }
}

व्योम vvp_fun_signal4_sa::recv_vec8_pv(vvp_net_ptr_t ptr, स्थिर vvp_vector8_t&bit,
				      अचिन्हित base, अचिन्हित vwid)
{
      recv_vec4_pv(ptr, reduce4(bit), base, vwid, 0);
}

व्योम vvp_fun_signal_base::deassign()
{
      continuous_assign_active_ = false;
      assign_mask_ = vvp_vector2_t();
}

व्योम vvp_fun_signal_base::deassign_pv(अचिन्हित base, अचिन्हित wid)
{
      क्रम (अचिन्हित idx = 0 ;  idx < wid ;  idx += 1) {
	    assign_mask_.set_bit(base+idx, 0);
      }

      अगर (assign_mask_.is_zero()) {
	    assign_mask_ = vvp_vector2_t();
      }
}

व्योम automatic_signal_base::release(vvp_net_ptr_t,bool)
{
      निश्चित(0);
}

व्योम automatic_signal_base::release_pv(vvp_net_ptr_t,अचिन्हित,अचिन्हित,bool)
{
      निश्चित(0);
}

अचिन्हित automatic_signal_base::filter_आकार() स्थिर
{
      निश्चित(0);
      वापस(0);
}
व्योम automatic_signal_base::force_fil_vec4(स्थिर vvp_vector4_t&, स्थिर vvp_vector2_t&)
{
      निश्चित(0);
}
व्योम automatic_signal_base::force_fil_vec8(स्थिर vvp_vector8_t&, स्थिर vvp_vector2_t&)
{
      निश्चित(0);
}
व्योम automatic_signal_base::force_fil_real(द्विगुन, स्थिर vvp_vector2_t&)
{
      निश्चित(0);
}
व्योम automatic_signal_base::get_मूल्य(काष्ठा t_vpi_मूल्य*)
{
      निश्चित(0);
}

स्थिर vvp_vector4_t& vvp_fun_signal4_sa::vec4_unfiltered_मूल्य() स्थिर
{
      वापस bits4_;
}

vvp_fun_signal4_aa::vvp_fun_signal4_aa(अचिन्हित wid, vvp_bit4_t init)
{
      context_idx_ = vpip_add_item_to_context(यह, vpip_peek_context_scope());
      आकार_2 = wid;
      init_ = init;
}

vvp_fun_signal4_aa::~vvp_fun_signal4_aa()
{
      निश्चित(0);
}

व्योम vvp_fun_signal4_aa::alloc_instance(vvp_context_t context)
{
      vvp_set_context_item(context, context_idx_, नया vvp_vector4_t(आकार_2, init_));
}

व्योम vvp_fun_signal4_aa::reset_instance(vvp_context_t context)
{
      vvp_vector4_t*bits = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      bits->fill_bits(init_);
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम vvp_fun_signal4_aa::free_instance(vvp_context_t context)
{
      vvp_vector4_t*bits = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));
      मिटाओ bits;
}
#पूर्ण_अगर

/*
 * Continuous and forced assignments are not permitted on automatic
 * variables. So we only expect to receive on port 0.
 */
व्योम vvp_fun_signal4_aa::recv_vec4(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
                                   vvp_context_t context)
{
      निश्चित(ptr.port() == 0);
      निश्चित(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      अगर (!bits4->eeq(bit)) {
            *bits4 = bit;
            ptr.ptr()->send_vec4(*bits4, context);
      }
}

व्योम vvp_fun_signal4_aa::recv_vec4_pv(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
				      अचिन्हित base, अचिन्हित vwid, vvp_context_t context)
{
      निश्चित(ptr.port() == 0);
      निश्चित(आकार_2 == vwid);
      निश्चित(context);

      vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vvp_get_context_item(context, context_idx_));

      अचिन्हित wid = bit.size();
      क्रम (अचिन्हित idx = 0 ;  idx < wid ;  idx += 1) {
            अगर (base+idx >= bits4->size()) अवरोध;
            bits4->set_bit(base+idx, bit.value(idx));
      }
      ptr.ptr()->send_vec4(*bits4, context);
}

अचिन्हित vvp_fun_signal4_aa::मूल्य_आकार() स्थिर
{
      वापस आकार_2;
}

vvp_bit4_t vvp_fun_signal4_aa::value(अचिन्हित idx) स्थिर
{
      स्थिर vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      वापस bits4->value(idx);
}

vvp_scalar_t vvp_fun_signal4_aa::scalar_मूल्य(अचिन्हित idx) स्थिर
{
      स्थिर vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      वापस vvp_scalar_t(bits4->value(idx), 6, 6);
}

व्योम vvp_fun_signal4_aa::vec4_मूल्य(vvp_vector4_t&val) स्थिर
{
      स्थिर vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      val = *bits4;
}

स्थिर vvp_vector4_t&vvp_fun_signal4_aa::vec4_unfiltered_मूल्य() स्थिर
{
      स्थिर vvp_vector4_t*bits4 = static_cast<vvp_vector4_t*>
            (vthread_get_rd_context_item(context_idx_));

      वापस *bits4;
}

व्योम vvp_fun_signal4_aa::चालक मिटाओ(व्योम*)
{
      निश्चित(0);
}

/*
 * Testing for equality, we want a bitwise test instead of an
 * arithmetic test because we want to treat for example -0 different
 * from +0.
 */
bool bits_equal(द्विगुन a, द्विगुन b)
{
      वापस स_भेद(&a, &b, माप a) == 0;
}

vvp_fun_signal_real_sa::vvp_fun_signal_real_sa()
{
      bits_ = 0.0;
}

द्विगुन vvp_fun_signal_real_sa::real_unfiltered_मूल्य() स्थिर
{
      वापस bits_;
}

व्योम vvp_fun_signal_real_sa::recv_real(vvp_net_ptr_t ptr, द्विगुन bit,
                                       vvp_context_t)
{
      चयन (ptr.port()) {
	  हाल 0:
	    अगर (!continuous_assign_active_) {
                  अगर (needs_init_ || !bits_equal(bits_, bit)) {
			bits_ = bit;
			needs_init_ = false;
			ptr.ptr()->send_real(bit, 0);
		  }
	    }
	    अवरोध;

	  हाल 1: // Continuous assign value
	    continuous_assign_active_ = true;
	    bits_ = bit;
	    ptr.ptr()->send_real(bit, 0);
	    अवरोध;

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "Unsupported port type %u.\n", ptr.port());
	    निश्चित(0);
	    अवरोध;
      }
}

vvp_fun_signal_real_aa::vvp_fun_signal_real_aa()
{
      context_idx_ = vpip_add_item_to_context(यह, vpip_peek_context_scope());
}

vvp_fun_signal_real_aa::~vvp_fun_signal_real_aa()
{
      निश्चित(0);
}

व्योम vvp_fun_signal_real_aa::alloc_instance(vvp_context_t context)
{
      द्विगुन*bits = नया द्विगुन;
      vvp_set_context_item(context, context_idx_, bits);

      *bits = 0.0;
}

व्योम vvp_fun_signal_real_aa::reset_instance(vvp_context_t context)
{
      द्विगुन*bits = static_cast<द्विगुन*>
            (vvp_get_context_item(context, context_idx_));

      *bits = 0.0;
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम vvp_fun_signal_real_aa::free_instance(vvp_context_t context)
{
      द्विगुन*bits = static_cast<द्विगुन*>
            (vvp_get_context_item(context, context_idx_));
      मिटाओ bits;
}
#पूर्ण_अगर

द्विगुन vvp_fun_signal_real_aa::real_unfiltered_मूल्य() स्थिर
{
      स्थिर द्विगुन*bits = static_cast<द्विगुन*>
            (vthread_get_rd_context_item(context_idx_));

      वापस *bits;
}

द्विगुन vvp_fun_signal_real_aa::real_मूल्य() स्थिर
{
      वापस real_unfiltered_मूल्य();
}

व्योम vvp_fun_signal_real_aa::recv_real(vvp_net_ptr_t ptr, द्विगुन bit,
                                       vvp_context_t context)
{
      निश्चित(ptr.port() == 0);
      निश्चित(context);

      द्विगुन*bits = static_cast<द्विगुन*>
            (vvp_get_context_item(context, context_idx_));

      अगर (!bits_equal(*bits,bit)) {
            *bits = bit;
            ptr.ptr()->send_real(bit, context);
      }
}

अचिन्हित vvp_fun_signal_real_aa::मूल्य_आकार() स्थिर
{
      निश्चित(0);
      वापस 1;
}

vvp_bit4_t vvp_fun_signal_real_aa::value(अचिन्हित) स्थिर
{
      निश्चित(0);
      वापस BIT4_X;
}

vvp_scalar_t vvp_fun_signal_real_aa::scalar_मूल्य(अचिन्हित) स्थिर
{
      निश्चित(0);
      वापस vvp_scalar_t();
}

व्योम vvp_fun_signal_real_aa::vec4_मूल्य(vvp_vector4_t&) स्थिर
{
      निश्चित(0);
}

व्योम* vvp_fun_signal_real_aa::चालक नया(std::माप_प्रकार size)
{
      वापस vvp_net_fun_t::heap_.alloc(size);
}

व्योम vvp_fun_signal_real_aa::चालक मिटाओ(व्योम*)
{
      निश्चित(0);
}


vvp_fun_signal_string_sa::vvp_fun_signal_string_sa()
{
}

व्योम vvp_fun_signal_string_sa::recv_string(vvp_net_ptr_t ptr, स्थिर std::string&bit,
					   vvp_context_t)
{
      निश्चित(ptr.port() == 0);

      अगर (needs_init_ || मूल्य_2 != bit) {
	    मूल्य_2 = bit;
	    needs_init_ = false;

	    ptr.ptr()->send_string(bit, 0);
      }
}

स्थिर string& vvp_fun_signal_string_sa::get_string() स्थिर
{
      वापस मूल्य_2;
}

vvp_fun_signal_string_aa::vvp_fun_signal_string_aa()
{
      context_idx_ = vpip_add_item_to_context(यह, vpip_peek_context_scope());
}

vvp_fun_signal_string_aa::~vvp_fun_signal_string_aa()
{
      निश्चित(0);
}

व्योम vvp_fun_signal_string_aa::alloc_instance(vvp_context_t context)
{
      string*bits = नया std::string;
      vvp_set_context_item(context, context_idx_, bits);
      *bits = "";
}

व्योम vvp_fun_signal_string_aa::reset_instance(vvp_context_t context)
{
      string*bits = static_cast<std::string*>
	    (vvp_get_context_item(context, context_idx_));
      *bits = "";
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम vvp_fun_signal_string_aa::free_instance(vvp_context_t context)
{
      string*bits = static_cast<std::string*>
            (vvp_get_context_item(context, context_idx_));
      मिटाओ bits;
}
#पूर्ण_अगर

व्योम vvp_fun_signal_string_aa::recv_string(vvp_net_ptr_t ptr, स्थिर std::string&bit, vvp_context_t context)
{
      निश्चित(ptr.port() == 0);
      निश्चित(context);

      string*bits = static_cast<std::string*>
	    (vvp_get_context_item(context, context_idx_));

      अगर (*bits != bit) {
	    *bits = bit;
	    ptr.ptr()->send_string(bit, context);
      }
}

अचिन्हित vvp_fun_signal_string_aa::मूल्य_आकार() स्थिर
{
      निश्चित(0);
      वापस 1;
}

vvp_bit4_t vvp_fun_signal_string_aa::value(अचिन्हित) स्थिर
{
      निश्चित(0);
      वापस BIT4_X;
}

vvp_scalar_t vvp_fun_signal_string_aa::scalar_मूल्य(अचिन्हित) स्थिर
{
      निश्चित(0);
      वापस vvp_scalar_t();
}

व्योम vvp_fun_signal_string_aa::vec4_मूल्य(vvp_vector4_t&) स्थिर
{
      निश्चित(0);
}

द्विगुन vvp_fun_signal_string_aa::real_मूल्य() स्थिर
{
      निश्चित(0);
      वापस 0.0;
}

स्थिर std::string& vvp_fun_signal_string_aa::get_string() स्थिर
{
      स्थिर string*bits = static_cast<std::string*>
            (vthread_get_rd_context_item(context_idx_));

      वापस *bits;
}

व्योम* vvp_fun_signal_string_aa::चालक नया(std::माप_प्रकार size)
{
      वापस vvp_net_fun_t::heap_.alloc(size);
}

व्योम vvp_fun_signal_string_aa::चालक मिटाओ(व्योम*)
{
      निश्चित(0);
}

  /* OBJECT signals */

vvp_fun_signal_object_sa::vvp_fun_signal_object_sa(अचिन्हित size)
: vvp_fun_signal_object(size)
{
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम vvp_fun_signal_object_aa::free_instance(vvp_context_t context)
{
      vvp_object_t*bits = static_cast<vvp_object_t*>
            (vvp_get_context_item(context, context_idx_));
      मिटाओ bits;
}
#पूर्ण_अगर

व्योम vvp_fun_signal_object_sa::recv_object(vvp_net_ptr_t ptr, vvp_object_t bit,
					   vvp_context_t)
{
      निश्चित(ptr.port() == 0);

      अगर (needs_init_ || मूल्य_2 != bit) {
	    मूल्य_2 = bit;
	    needs_init_ = false;

	    ptr.ptr()->send_object(bit, 0);
      }
}

vvp_object_t vvp_fun_signal_object_sa::get_object() स्थिर
{
      वापस मूल्य_2;
}

vvp_fun_signal_object_aa::vvp_fun_signal_object_aa(अचिन्हित size)
: vvp_fun_signal_object(size)
{
      context_idx_ = vpip_add_item_to_context(यह, vpip_peek_context_scope());
}

vvp_fun_signal_object_aa::~vvp_fun_signal_object_aa()
{
      निश्चित(0);
}

व्योम vvp_fun_signal_object_aa::alloc_instance(vvp_context_t context)
{
      vvp_object_t*bits = नया vvp_object_t;
      vvp_set_context_item(context, context_idx_, bits);
      bits->reset();
}

व्योम vvp_fun_signal_object_aa::reset_instance(vvp_context_t context)
{
      vvp_object_t*bits = static_cast<vvp_object_t*>
	    (vvp_get_context_item(context, context_idx_));
      bits->reset();
}

vvp_object_t vvp_fun_signal_object_aa::get_object() स्थिर
{
      स्थिर vvp_object_t*bits = static_cast<vvp_object_t*>
	    (vthread_get_rd_context_item(context_idx_));
      वापस *bits;
}

व्योम vvp_fun_signal_object_aa::recv_object(vvp_net_ptr_t ptr, vvp_object_t bit,
					   vvp_context_t context)
{
      निश्चित(ptr.port() == 0);
      निश्चित(context);

      vvp_object_t*bits = static_cast<vvp_object_t*>
	    (vvp_get_context_item(context, context_idx_));

      अगर (*bits != bit) {
	    *bits = bit;
	    ptr.ptr()->send_object(bit, context);
      }
}

अचिन्हित vvp_fun_signal_object_aa::मूल्य_आकार() स्थिर
{
      वापस 1;
}

vvp_bit4_t vvp_fun_signal_object_aa::value(अचिन्हित) स्थिर
{
      निश्चित(0);
      वापस BIT4_X;
}

vvp_scalar_t vvp_fun_signal_object_aa::scalar_मूल्य(अचिन्हित) स्थिर
{
      निश्चित(0);
      वापस vvp_scalar_t();
}

व्योम vvp_fun_signal_object_aa::vec4_मूल्य(vvp_vector4_t&) स्थिर
{
      निश्चित(0);
}

व्योम* vvp_fun_signal_object_aa::चालक नया(std::माप_प्रकार size)
{
      वापस vvp_net_fun_t::heap_.alloc(size);
}

व्योम vvp_fun_signal_object_aa::चालक मिटाओ(व्योम*)
{
      निश्चित(0);
}

  /* **** */

vvp_fun_force::vvp_fun_force()
{
}

vvp_fun_force::~vvp_fun_force()
{
}

व्योम vvp_fun_force::recv_vec4(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
			      vvp_context_t)
{
      निश्चित(ptr.port() == 0);
      vvp_net_t*net = ptr.ptr();

      vvp_net_t*dst = net->port[3].ptr();
      निश्चित(dst->fil);

      dst->force_vec4(coerce_to_width(bit, dst->fil->filter_आकार()), vvp_vector2_t(vvp_vector2_t::FILL1, dst->fil->filter_आकार()));
}

व्योम vvp_fun_force::recv_real(vvp_net_ptr_t ptr, द्विगुन bit, vvp_context_t)
{
      निश्चित(ptr.port() == 0);
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

vvp_bit4_t vvp_wire_base::driven_मूल्य(अचिन्हित) स्थिर
{
      निश्चित(0);
      वापस BIT4_X;
}

bool vvp_wire_base::is_forced(अचिन्हित) स्थिर
{
      निश्चित(0);
      वापस false;
}

vvp_wire_vec4::vvp_wire_vec4(अचिन्हित wid, vvp_bit4_t init)
: bits4_(wid, init)
{
      needs_init_ = true;
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec4(स्थिर vvp_vector4_t&bit, vvp_vector4_t&rep,
						 अचिन्हित base, अचिन्हित vwid)
{
	// Special case! the input bit is 0 wid. Interpret this as a
	// vector of BIT4_X to match the width of the bits4_ vector.
	// FIXME! This is a hack to work around some buggy gate
	// implementations! This should be removed!
      अगर (base==0 && vwid==0) {
	    vvp_vector4_t tmp (bits4_.size(), BIT4_X);
	    अगर (bits4_ .eeq(tmp) && !needs_init_) वापस STOP;
	    bits4_ = tmp;
	    needs_init_ = false;
	    वापस filter_mask_(tmp, force4_, rep, 0);
      }

      अगर (vwid != bits4_.size()) {
	    अ_त्रुटि << "Internal error: Input vector expected width="
		 << bits4_.size() << ", got "
		 << "bit=" << bit << ", base=" << base << ", vwid=" << vwid
		 << प_पूर्ण;
      }
      निश्चित(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      अगर (base==0 && bit.size()==vwid) {
	    अगर (bits4_ .eeq( bit ) && !needs_init_) वापस STOP;
	    bits4_ = bit;
      } अन्यथा {
	    bool rc = bits4_.set_vec(base, bit);
	    अगर (rc == false && !needs_init_) वापस STOP;
      }

      needs_init_ = false;
      वापस filter_mask_(bit, force4_, rep, base);
}

vvp_net_fil_t::prop_t vvp_wire_vec4::filter_vec8(स्थिर vvp_vector8_t&bit,
                                                 vvp_vector8_t&rep,
                                                 अचिन्हित base,
                                                 अचिन्हित vwid)
{
      निश्चित(bits4_.size() == vwid);

	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      vvp_vector4_t bit4 (reduce4(bit));
      अगर (base==0 && bit4.size()==vwid) {
	    अगर (bits4_ .eeq( bit4 ) && !needs_init_) वापस STOP;
	    bits4_ = bit4;
      } अन्यथा {
	    bool rc = bits4_.set_vec(base, bit4);
	    अगर (rc == false && !needs_init_) वापस STOP;
      }

      needs_init_ = false;
      वापस filter_mask_(bit, vvp_vector8_t(force4_,6,6), rep, base);
}

अचिन्हित vvp_wire_vec4::filter_आकार() स्थिर
{
      वापस bits4_.size();
}

व्योम vvp_wire_vec4::force_fil_vec4(स्थिर vvp_vector4_t&val, स्थिर vvp_vector2_t&mask)
{
      force_mask(mask);

      अगर (force4_.size() == 0) {
	    force4_ = val;
      } अन्यथा {
	    क्रम (अचिन्हित idx = 0; idx < mask.size() ; idx += 1) {
		  अगर (mask.value(idx) == 0)
			जारी;

		  force4_.set_bit(idx, val.value(idx));
	    }
      }
      run_vpi_callbacks();
}

व्योम vvp_wire_vec4::force_fil_vec8(स्थिर vvp_vector8_t&, स्थिर vvp_vector2_t&)
{
      निश्चित(0);
}

व्योम vvp_wire_vec4::force_fil_real(द्विगुन, स्थिर vvp_vector2_t&)
{
      निश्चित(0);
}

व्योम vvp_wire_vec4::release(vvp_net_ptr_t ptr, bool net_flag)
{
      vvp_vector2_t mask (vvp_vector2_t::FILL1, bits4_.size());
      अगर (net_flag) {
	      // Wires revert to their unforced value after release.
            release_mask(mask);
	    needs_init_ = ! force4_ .eeq(bits4_);
	    ptr.ptr()->send_vec4(bits4_, 0);
	    run_vpi_callbacks();
      } अन्यथा {
	      // Variables keep the current value.
	    vvp_vector4_t res (bits4_.size());
	    क्रम (अचिन्हित idx=0; idx<bits4_.size(); idx += 1)
		  res.set_bit(idx,value(idx));
            release_mask(mask);
	    ptr.ptr()->fun->recv_vec4(ptr, res, 0);
      }
}

व्योम vvp_wire_vec4::release_pv(vvp_net_ptr_t ptr, अचिन्हित base, अचिन्हित wid, bool net_flag)
{
      निश्चित(bits4_.size() >= base + wid);

      vvp_vector2_t mask (vvp_vector2_t::FILL0, bits4_.size());
      क्रम (अचिन्हित idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      अगर (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_mask(mask);
	    needs_init_ = ! force4_.subvalue(base,wid) .eeq(bits4_.subvalue(base,wid));
	    ptr.ptr()->send_vec4_pv(bits4_.subvalue(base,wid),
				    base, bits4_.size(), 0);
	    run_vpi_callbacks();
      } अन्यथा {
	      // Variables keep the current value.
	    vvp_vector4_t res (wid);
	    क्रम (अचिन्हित idx=0; idx<wid; idx += 1)
		  res.set_bit(idx,value(base+idx));
	    release_mask(mask);
	    ptr.ptr()->fun->recv_vec4_pv(ptr, res, base, bits4_.size(), 0);
      }
}

अचिन्हित vvp_wire_vec4::मूल्य_आकार() स्थिर
{
      वापस bits4_.size();
}

vvp_bit4_t vvp_wire_vec4::filtered_मूल्य(अचिन्हित idx) स्थिर
{
      अगर (test_force_mask(idx))
	    वापस force4_.value(idx);
      अन्यथा
	    वापस bits4_.value(idx);
}

vvp_bit4_t vvp_wire_vec4::value(अचिन्हित idx) स्थिर
{
      वापस filtered_मूल्य(idx);
}

vvp_scalar_t vvp_wire_vec4::scalar_मूल्य(अचिन्हित idx) स्थिर
{
      वापस vvp_scalar_t(value(idx),6,6);
}

व्योम vvp_wire_vec4::vec4_मूल्य(vvp_vector4_t&val) स्थिर
{
      val = bits4_;
      अगर (test_force_mask_is_zero())
	    वापस;

      क्रम (अचिन्हित idx = 0 ; idx < bits4_.size() ; idx += 1)
	    val.set_bit(idx, filtered_मूल्य(idx));
}

vvp_bit4_t vvp_wire_vec4::driven_मूल्य(अचिन्हित idx) स्थिर
{
      वापस bits4_.value(idx);
}

bool vvp_wire_vec4::is_forced(अचिन्हित idx) स्थिर
{
      वापस test_force_mask(idx);
}

vvp_wire_vec8::vvp_wire_vec8(अचिन्हित wid)
: bits8_(wid)
{
      needs_init_ = true;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec4(स्थिर vvp_vector4_t&bit,
                                                 vvp_vector4_t&rep,
                                                 अचिन्हित base,
                                                 अचिन्हित vwid)
{
	// For now there is no support for a non-zero base.
      निश्चित(0 == base);
      निश्चित(bits8_.size() == vwid);
      निश्चित(bits8_.size() == bit.size());
	// QUESTION: Is it really correct to propagate a vec4 if this
	// is a vec8 node? In fact, it is really possible for a vec4
	// value to get through to a vec8 filter?
      vvp_vector8_t rep8;
      prop_t rc = filter_vec8(vvp_vector8_t(bit,6,6), rep8, 0, vwid);
      अगर (rc == REPL)
	    rep = reduce4(rep8);

      needs_init_ = false;
      वापस rc;
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_vec8(स्थिर vvp_vector8_t&bit, vvp_vector8_t&rep, अचिन्हित base, अचिन्हित vwid)
{
      निश्चित(vwid == bits8_.size());
	// Keep track of the value being driven from this net, even if
	// it is not ultimately what survives the force filter.
      अगर (base==0 && bit.size()==vwid) {
	    अगर (bits8_.eeq(bit) && !needs_init_) वापस STOP;
	    bits8_ = bit;
      } अन्यथा {
	    अगर (bits8_.subvalue(base, bit.size()).eeq(bit) &&
		!needs_init_) {
		  वापस STOP;
	    }
	    अगर (bits8_.size() == 0)
		  bits8_ = vvp_vector8_t(vwid);
	    निश्चित(bits8_.size() == vwid);
	    bits8_.set_vec(base, bit);
      }
      needs_init_ = false;
      वापस filter_mask_(bit, force8_, rep, base);
}

vvp_net_fil_t::prop_t vvp_wire_vec8::filter_निवेश_vec8(स्थिर vvp_vector8_t&bit, vvp_vector8_t&rep) स्थिर
{
      वापस filter_निवेश_mask(bit, force8_, rep);
}

अचिन्हित vvp_wire_vec8::filter_आकार() स्थिर
{
      वापस bits8_.size();
}

व्योम vvp_wire_vec8::force_fil_vec4(स्थिर vvp_vector4_t&val, स्थिर vvp_vector2_t&mask)
{
      force_fil_vec8(vvp_vector8_t(val,6,6), mask);
}

व्योम vvp_wire_vec8::force_fil_vec8(स्थिर vvp_vector8_t&val, स्थिर vvp_vector2_t&mask)
{
      force_mask(mask);

      अगर (force8_.size() == 0) {
	    force8_ = val;
      } अन्यथा {
	    क्रम (अचिन्हित idx = 0; idx < mask.size() ; idx += 1) {
		  अगर (mask.value(idx) == 0)
			जारी;

		  force8_.set_bit(idx, val.value(idx));
	    }
      }
      run_vpi_callbacks();
}

व्योम vvp_wire_vec8::force_fil_real(द्विगुन, स्थिर vvp_vector2_t&)
{
      निश्चित(0);
}

व्योम vvp_wire_vec8::release(vvp_net_ptr_t ptr, bool net_flag)
{
	// Wires revert to their unforced value after release.
      vvp_vector2_t mask (vvp_vector2_t::FILL1, bits8_.size());
      release_mask(mask);
      अगर (net_flag) {
	    needs_init_ = !force8_ .eeq(bits8_);
	    ptr.ptr()->send_vec8(bits8_);
      } अन्यथा {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    निश्चित(0);
//	    ptr.ptr()->fun->recv_vec8(ptr, force8_);
      }
}

व्योम vvp_wire_vec8::release_pv(vvp_net_ptr_t ptr, अचिन्हित base, अचिन्हित wid, bool net_flag)
{
      निश्चित(bits8_.size() >= base + wid);

      vvp_vector2_t mask (vvp_vector2_t::FILL0, bits8_.size());
      क्रम (अचिन्हित idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      release_mask(mask);

      अगर (net_flag) {
	    needs_init_ = !force8_.subvalue(base,wid) .eeq((bits8_.subvalue(base,wid)));
	    ptr.ptr()->send_vec8_pv(bits8_.subvalue(base,wid),
				    base, bits8_.size());
	    run_vpi_callbacks();
      } अन्यथा {
	// Variable do not know about strength so this should not be able
	// to happen. If for some reason it can then it should not be too
	// hard to fix this code like was done for vvp_wire_vec4 above.
	    निश्चित(0);
//	    ptr.ptr()->fun->recv_vec8_pv(ptr, force8_.subvalue(base,wid),
//					 base, force8_.size());
      }
}

अचिन्हित vvp_wire_vec8::मूल्य_आकार() स्थिर
{
      वापस bits8_.size();
}

vvp_scalar_t vvp_wire_vec8::filtered_मूल्य(अचिन्हित idx) स्थिर
{
      अगर (test_force_mask(idx))
	    वापस force8_.value(idx);
      अन्यथा
	    वापस bits8_.value(idx);
}

vvp_bit4_t vvp_wire_vec8::value(अचिन्हित idx) स्थिर
{
      वापस filtered_मूल्य(idx).value();
}

vvp_scalar_t vvp_wire_vec8::scalar_मूल्य(अचिन्हित idx) स्थिर
{
      वापस filtered_मूल्य(idx);
}

vvp_vector8_t vvp_wire_vec8::vec8_मूल्य() स्थिर
{
      vvp_vector8_t tmp = bits8_;
      क्रम (अचिन्हित idx = 0 ; idx < bits8_.size() ; idx += 1)
	    tmp.set_bit(idx, filtered_मूल्य(idx));
      वापस tmp;
}

व्योम vvp_wire_vec8::vec4_मूल्य(vvp_vector4_t&val) स्थिर
{
      val = reduce4(vec8_मूल्य());
}

vvp_bit4_t vvp_wire_vec8::driven_मूल्य(अचिन्हित idx) स्थिर
{
      वापस bits8_.value(idx).value();
}

bool vvp_wire_vec8::is_forced(अचिन्हित idx) स्थिर
{
      वापस test_force_mask(idx);
}

vvp_wire_real::vvp_wire_real()
: bit_(0.0), force_(0.0)
{
}

vvp_net_fil_t::prop_t vvp_wire_real::filter_real(द्विगुन&bit)
{
      अगर (bits_equal(bit_, bit) && !needs_init_) वापस STOP;
      bit_ = bit;
      needs_init_ = false;
      वापस filter_mask_(bit, force_);
}

अचिन्हित vvp_wire_real::filter_आकार() स्थिर
{
      निश्चित(0);
      वापस 0;
}

व्योम vvp_wire_real::force_fil_vec4(स्थिर vvp_vector4_t&, स्थिर vvp_vector2_t&)
{
      निश्चित(0);
}

व्योम vvp_wire_real::force_fil_vec8(स्थिर vvp_vector8_t&, स्थिर vvp_vector2_t&)
{
      निश्चित(0);
}

व्योम vvp_wire_real::force_fil_real(द्विगुन val, स्थिर vvp_vector2_t&mask)
{
      force_mask(mask);
      अगर (mask.value(0))
	    force_ = val;

      run_vpi_callbacks();
}

व्योम vvp_wire_real::release(vvp_net_ptr_t ptr, bool net_flag)
{
      vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
      अगर (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_mask(mask);
	    needs_init_ = !bits_equal(force_, bit_);
	    ptr.ptr()->send_real(bit_, 0);
      } अन्यथा {
	      // Variables keep the current value.
	    द्विगुन res =  real_मूल्य();
	    release_mask(mask);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
}

व्योम vvp_wire_real::release_pv(vvp_net_ptr_t, अचिन्हित, अचिन्हित, bool)
{
      निश्चित(0);
#अगर 0
	// A real is a single value. If for some reason this part release
	// can happen the following code should work correctly (requires
	// a base of 0 and a width of 1).
      vvp_vector2_t mask (vvp_vector2_t::FILL1, 1);
      निश्चित(base == 0 && wid == 1);

      अगर (net_flag) {
	      // Wires revert to their unforced value after release.
	    release_mask(mask);
	    ptr.ptr()->send_real(bit_, 0);
      } अन्यथा {
	      // Variables keep the current value.
	    द्विगुन res =  real_मूल्य();
	    release_mask(mask);
	    ptr.ptr()->fun->recv_real(ptr, res, 0);
      }
#पूर्ण_अगर
}

अचिन्हित vvp_wire_real::मूल्य_आकार() स्थिर
{
      निश्चित(0);
      वापस 1;
}

vvp_bit4_t vvp_wire_real::value(अचिन्हित) स्थिर
{
      निश्चित(0);
      वापस BIT4_X;
}

vvp_scalar_t vvp_wire_real::scalar_मूल्य(अचिन्हित) स्थिर
{
      निश्चित(0);
      वापस vvp_scalar_t();
}

व्योम vvp_wire_real::vec4_मूल्य(vvp_vector4_t&) स्थिर
{
      निश्चित(0);
}

द्विगुन vvp_wire_real::real_मूल्य() स्थिर
{
      अगर (test_force_mask(0))
	    वापस force_;
      अन्यथा
	    वापस bit_;
}

#अगर 0
vvp_wire_string::vvp_wire_string()
{
}

अचिन्हित vvp_wire_string::filter_आकार() स्थिर
{
      निश्चित(0);
      वापस 0;
}

व्योम vvp_wire_string::force_fil_vec4(स्थिर vvp_vector4_t&, स्थिर vvp_vector2_t&)
{
      निश्चित(0);
}
व्योम vvp_wire_string::force_fil_vec8(स्थिर vvp_vector8_t&, स्थिर vvp_vector2_t&)
{
      निश्चित(0);
}
व्योम vvp_wire_string::force_fil_real(द्विगुन, स्थिर vvp_vector2_t&)
{
      निश्चित(0);
}

व्योम vvp_wire_string::release(vvp_net_ptr_t ptr, bool net_flag)
{
      निश्चित(0);
}

व्योम vvp_wire_string::release_pv(vvp_net_ptr_t, अचिन्हित, अचिन्हित, bool)
{
      निश्चित(0);
}

अचिन्हित vvp_wire_string::मूल्य_आकार() स्थिर
{
      निश्चित(0);
      वापस 1;
}

vvp_bit4_t vvp_wire_string::value(अचिन्हित) स्थिर
{
      निश्चित(0);
      वापस BIT4_X;
}

vvp_scalar_t vvp_wire_string::scalar_मूल्य(अचिन्हित) स्थिर
{
      निश्चित(0);
      वापस vvp_scalar_t();
}

व्योम vvp_wire_string::vec4_मूल्य(vvp_vector4_t&) स्थिर
{
      निश्चित(0);
}

द्विगुन vvp_wire_string::real_मूल्य() स्थिर
{
      निश्चित(0);
      वापस 0.0;
}
#पूर्ण_अगर
