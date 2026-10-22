#ifndef IVL_vvp_net_sig_H
#define IVL_vvp_net_sig_H
/*
 * Copyright (c) 2004-2026 Stephen Williams (steve@icarus.com)
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
# include  "vpi_user.h"
# include  "vvp_net.h"
# include  "vvp_object.h"
# include  <string>
# include  <cstddef>
# include  <cstdlib>
# include  <cstring>
# include  <new>
# include  <cassert>

#ifdef HAVE_IOSFWD
# include  <iosfwd>
#ಅನ್ಯಥಾ
# include  <iostream>
#endif

/* vvp_fun_signal
 * This node is the place holder in a vvp network for signals,
 * including nets of various sort. The output from a signal follows
 * the type of its port-0 input. If vvp_vector4_t values come in
 * through port-0, then vvp_vector4_t values are propagated. If
 * vvp_vector8_t values come in through port-0, then vvp_vector8_t
 * values are propagated. Thus, this node is slightly polymorphic.
 *
 * If the signal is a net (i.e. a wire or tri) then this node will
 * have an input that is the data source. The data source will connect
 * through port-0.
 *
 * If the signal is a reg, then there will be no netlist input, the
 * values will be written by behavioral statements. The %set and
 * %assign statements will write through port-0.
 *
 * In any case, behavioral code is able to read the value that this
 * node last propagated, by using the value() method. That is important
 * functionality of this node.
 *
 * Continuous assignments are made through port-1. When a value is
 * written here, continuous assign mode is activated, and input
 * through port-0 is ignored until continuous assign mode is turned
 * off again. Writing into this port can be done in behavioral code
 * using the %cassign/v instruction, or can be done by the network by
 * hooking the output of a vvp_net_t to this port.
 */


class vvp_fun_signal_base : ಸಾರ್ವಜನಿಕ vvp_net_fun_t {

    ಸಾರ್ವಜನಿಕ:
      vvp_fun_signal_base();

      ಶೂನ್ಯ deassign();
      ಶೂನ್ಯ deassign_pv(unsigned base, unsigned wid);

    ಸಾರ್ವಜನಿಕ:

	/* The %cassign/link instruction needs a place to write the
	   source node of the force, so that subsequent %cassign and
	   %deassign instructions can undo the link as needed. */
      class vvp_net_t*cassign_link;

    ರಕ್ಷಿತ:
      bool continuous_assign_active_;
      vvp_vector2_t assign_mask_;

    ರಕ್ಷಿತ:
	// This is true until at least one propagation happens.
      bool needs_init_;
};

/*
 * Variables and wires can have their values accessed, so this base
 * class offers the unified concept of an accessible value.
 */
class vvp_signal_ಮೌಲ್ಯ {
    ಸಾರ್ವಜನಿಕ:
      ಆಭಾಸ ~vvp_signal_ಮೌಲ್ಯ() =0;
      ಆಭಾಸ unsigned ಮೌಲ್ಯ_size() ಧ್ರುವ =0;
      ಆಭಾಸ vvp_bit4_t ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ =0;
      ಆಭಾಸ vvp_scalar_t scalar_ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ =0;
      ಆಭಾಸ ಶೂನ್ಯ vec4_ಮೌಲ್ಯ(vvp_vector4_t&) ಧ್ರುವ =0;
      ಆಭಾಸ ದ್ವಿಗುಣ real_ಮೌಲ್ಯ() ಧ್ರುವ;

      ಆಭಾಸ ಶೂನ್ಯ get_signal_ಮೌಲ್ಯ(ಸಂರಚನಾ t_vpi_ಮೌಲ್ಯ*vp);
};

/*
 * This abstract class is a little more specific than the signal_base
 * class, in that it adds vector access methods.
 */
class vvp_fun_signal_vec : ಸಾರ್ವಜನಿಕ vvp_fun_signal_base {
    ಸಾರ್ವಜನಿಕ:
      ಆಭಾಸ ಧ್ರುವ vvp_vector4_t& vec4_unfiltered_ಮೌಲ್ಯ() ಧ್ರುವ =0;
};

class automatic_signal_base : ಸಾರ್ವಜನಿಕ vvp_signal_ಮೌಲ್ಯ, ಸಾರ್ವಜನಿಕ vvp_net_fil_t {

    ಸಾರ್ವಜನಿಕ:
	// Automatic variables cannot be forced or released. Provide
	// stubs that assert.
      ಆಭಾಸ ಶೂನ್ಯ release(vvp_net_ptr_t ptr, bool net_flag) override;
      ಆಭಾಸ ಶೂನ್ಯ release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

      ಆಭಾಸ unsigned filter_size() ಧ್ರುವ override;
      ಆಭಾಸ ಶೂನ್ಯ force_fil_vec4(ಧ್ರುವ vvp_vector4_t&val, ಧ್ರುವ vvp_vector2_t&mask) override;
      ಆಭಾಸ ಶೂನ್ಯ force_fil_vec8(ಧ್ರುವ vvp_vector8_t&val, ಧ್ರುವ vvp_vector2_t&mask) override;
      ಆಭಾಸ ಶೂನ್ಯ force_fil_real(ದ್ವಿಗುಣ val, ಧ್ರುವ vvp_vector2_t&mask) override;
      ಆಭಾಸ ಶೂನ್ಯ get_ಮೌಲ್ಯ(ಸಂರಚನಾ t_vpi_ಮೌಲ್ಯ*ಮೌಲ್ಯ) override;
};

/*
 * Statically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_sa : ಸಾರ್ವಜನಿಕ vvp_fun_signal_vec {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_signal4_sa(unsigned wid, vvp_bit4_t init=BIT4_X);

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t) override;
      ಶೂನ್ಯ recv_vec8(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector8_t&bit) override;

	// Part select variants of above
      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t) override;
      ಶೂನ್ಯ recv_vec8_pv(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector8_t&bit,
			unsigned base, unsigned vwid) override;

	// Get information about the vector value.
      ಧ್ರುವ vvp_vector4_t& vec4_unfiltered_ಮೌಲ್ಯ() ಧ್ರುವ override;

    ಗುಪ್ತ:
      vvp_vector4_t bits4_;
};

/*
 * Automatically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_aa : ಸಾರ್ವಜನಿಕ vvp_fun_signal_vec, ಸಾರ್ವಜನಿಕ automatic_signal_base, ಸಾರ್ವಜನಿಕ automatic_hooks_s {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_signal4_aa(unsigned wid, vvp_bit4_t init=BIT4_X);
      ~vvp_fun_signal4_aa() override;

      ಶೂನ್ಯ alloc_instance(vvp_context_t context) override;
      ಶೂನ್ಯ reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      ಶೂನ್ಯ free_instance(vvp_context_t context) override;
#endif

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t context) override;

	// Part select variants of above
      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t) override;

	// Get information about the vector value.
      unsigned   ಮೌಲ್ಯ_size() ಧ್ರುವ override;
      vvp_bit4_t ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ override;
      vvp_scalar_t scalar_ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ override;
      ಶೂನ್ಯ vec4_ಮೌಲ್ಯ(vvp_vector4_t&) ಧ್ರುವ override;
      ಧ್ರುವ vvp_vector4_t& vec4_unfiltered_ಮೌಲ್ಯ() ಧ್ರುವ override;

    ಸಾರ್ವಜನಿಕ: // These objects are only permallocated.
      ಸ್ಥಿರ ಶೂನ್ಯ* operator new(std::size_t size) { ಪ್ರತ್ಯಾಗಮ vvp_net_fun_t::heap_.alloc(size); }
      ಸ್ಥಿರ ಶೂನ್ಯ operator delete(ಶೂನ್ಯ*obj);

    ಗುಪ್ತ:
      unsigned context_idx_;
      unsigned size_;
      vvp_bit4_t init_;
};

class vvp_fun_signal_real : ಸಾರ್ವಜನಿಕ vvp_fun_signal_base {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_signal_real() {};

	// Get information about the vector value.
      ಆಭಾಸ ದ್ವಿಗುಣ real_unfiltered_ಮೌಲ್ಯ() ಧ್ರುವ = 0;
};

/*
 * Statically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_sa : ಸಾರ್ವಜನಿಕ vvp_fun_signal_real {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_signal_real_sa();

      ಶೂನ್ಯ recv_real(vvp_net_ptr_t port, ದ್ವಿಗುಣ bit,
                     vvp_context_t) override;

	// Get information about the vector value.
      ದ್ವಿಗುಣ real_unfiltered_ಮೌಲ್ಯ() ಧ್ರುವ override;

    ಗುಪ್ತ:
      ದ್ವಿಗುಣ bits_;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_aa : ಸಾರ್ವಜನಿಕ vvp_fun_signal_real, ಸಾರ್ವಜನಿಕ automatic_signal_base, ಸಾರ್ವಜನಿಕ automatic_hooks_s {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_signal_real_aa();
      ~vvp_fun_signal_real_aa() override;

      ಶೂನ್ಯ alloc_instance(vvp_context_t context) override;
      ಶೂನ್ಯ reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      ಶೂನ್ಯ free_instance(vvp_context_t context) override;
#endif

      ಶೂನ್ಯ recv_real(vvp_net_ptr_t port, ದ್ವಿಗುಣ bit,
                     vvp_context_t context) override;

	// Get information about the vector value.
      ದ್ವಿಗುಣ real_unfiltered_ಮೌಲ್ಯ() ಧ್ರುವ override;

	// Get information about the vector value.
      unsigned   ಮೌಲ್ಯ_size() ಧ್ರುವ override;
      vvp_bit4_t ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ override;
      vvp_scalar_t scalar_ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ override;
      ಶೂನ್ಯ vec4_ಮೌಲ್ಯ(vvp_vector4_t&) ಧ್ರುವ override;
      ದ್ವಿಗುಣ real_ಮೌಲ್ಯ() ಧ್ರುವ override;
      ಶೂನ್ಯ get_signal_ಮೌಲ್ಯ(ಸಂರಚನಾ t_vpi_ಮೌಲ್ಯ*vp) override;

    ಸಾರ್ವಜನಿಕ: // These objects are only permallocated.
      ಸ್ಥಿರ ಶೂನ್ಯ* operator new(std::size_t size);
      ಸ್ಥಿರ ಶೂನ್ಯ operator delete(ಶೂನ್ಯ*obj);

    ಗುಪ್ತ:
      unsigned context_idx_;
};


class vvp_fun_signal_string : ಸಾರ್ವಜನಿಕ vvp_fun_signal_base {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_signal_string() {};

      ಆಭಾಸ ಧ್ರುವ std::string& get_string() ಧ್ರುವ =0;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_string_sa : ಸಾರ್ವಜನಿಕ vvp_fun_signal_string {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_signal_string_sa();

      ಶೂನ್ಯ recv_string(vvp_net_ptr_t port, ಧ್ರುವ std::string&bit,
		       vvp_context_t context) override;

      ಧ್ರುವ std::string& get_string() ಧ್ರುವ override;

    ಗುಪ್ತ:
      std::string ಮೌಲ್ಯ_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_string_aa : ಸಾರ್ವಜನಿಕ vvp_fun_signal_string, ಸಾರ್ವಜನಿಕ automatic_signal_base, ಸಾರ್ವಜನಿಕ automatic_hooks_s {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_signal_string_aa();
      ~vvp_fun_signal_string_aa() override;

      ಶೂನ್ಯ alloc_instance(vvp_context_t context) override;
      ಶೂನ್ಯ reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      ಶೂನ್ಯ free_instance(vvp_context_t context) override;
#endif
      ಶೂನ್ಯ recv_string(vvp_net_ptr_t port, ಧ್ರುವ std::string&bit,
		       vvp_context_t context) override;

	// Get information about the vector value.
      unsigned   ಮೌಲ್ಯ_size() ಧ್ರುವ override;
      vvp_bit4_t ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ override;
      vvp_scalar_t scalar_ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ override;
      ಶೂನ್ಯ vec4_ಮೌಲ್ಯ(vvp_vector4_t&) ಧ್ರುವ override;
      ದ್ವಿಗುಣ real_ಮೌಲ್ಯ() ಧ್ರುವ override;
      ಧ್ರುವ std::string& get_string() ಧ್ರುವ override;
      ಶೂನ್ಯ get_signal_ಮೌಲ್ಯ(ಸಂರಚನಾ t_vpi_ಮೌಲ್ಯ*vp) override;

    ಸಾರ್ವಜನಿಕ: // These objects are only permallocated.
      ಸ್ಥಿರ ಶೂನ್ಯ* operator new(std::size_t size);
      ಸ್ಥಿರ ಶೂನ್ಯ operator delete(ಶೂನ್ಯ*obj);

    ಗುಪ್ತ:
      unsigned context_idx_;
};

class vvp_fun_signal_object : ಸಾರ್ವಜನಿಕ vvp_fun_signal_base {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_signal_object(unsigned size) { size_ = size; };
      unsigned size() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ size_; }

      ಆಭಾಸ vvp_object_t get_object() ಧ್ರುವ =0;
    ಗುಪ್ತ:
      unsigned size_;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_object_sa : ಸಾರ್ವಜನಿಕ vvp_fun_signal_object {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_signal_object_sa(unsigned size);

      ಶೂನ್ಯ recv_object(vvp_net_ptr_t port, vvp_object_t bit,
		    vvp_context_t context) override;

      vvp_object_t get_object() ಧ್ರುವ override;

    ಗುಪ್ತ:
      vvp_object_t ಮೌಲ್ಯ_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_object_aa : ಸಾರ್ವಜನಿಕ vvp_fun_signal_object, ಸಾರ್ವಜನಿಕ automatic_signal_base, ಸಾರ್ವಜನಿಕ automatic_hooks_s {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_signal_object_aa(unsigned size);
      ~vvp_fun_signal_object_aa() override;

      ಶೂನ್ಯ alloc_instance(vvp_context_t context) override;
      ಶೂನ್ಯ reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      ಶೂನ್ಯ free_instance(vvp_context_t context) override;
#endif

      ಶೂನ್ಯ recv_object(vvp_net_ptr_t port, vvp_object_t bit,
		    vvp_context_t context) override;

	// Get information about the vector value.
      unsigned   ಮೌಲ್ಯ_size() ಧ್ರುವ override;
      vvp_bit4_t ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ override;
      vvp_scalar_t scalar_ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ override;
      ಶೂನ್ಯ vec4_ಮೌಲ್ಯ(vvp_vector4_t&) ಧ್ರುವ override;
	//double real_value() const;
	//void get_signal_value(struct t_vpi_value*vp);

      vvp_object_t get_object() ಧ್ರುವ override;

    ಸಾರ್ವಜನಿಕ: // These objects are only permallocated.
      ಸ್ಥಿರ ಶೂನ್ಯ* operator new(std::size_t size);
      ಸ್ಥಿರ ಶೂನ್ಯ operator delete(ಶೂನ್ಯ*obj);

    ಗುಪ್ತ:
      unsigned context_idx_;
};


/* vvp_wire
 * The vvp_wire is different from vvp_variable objects in that it
 * exists only as a filter. The vvp_wire class tree is for
 * implementing Verilog wires/nets (as opposed to regs/variables).
 *
 *   vvp_vpi_callback
 *          |
 *          |
 *    vvp_net_fil_t   vvp_signal_value
 *          |               |
 *           \             /
 *            \           /
 *             \         /
 *            vvp_wire_base
 */

class vvp_wire_base  : ಸಾರ್ವಜನಿಕ vvp_net_fil_t, ಸಾರ್ವಜನಿಕ vvp_signal_ಮೌಲ್ಯ {

    ಸಾರ್ವಜನಿಕ:
      vvp_wire_base();
      ~vvp_wire_base() override;

        // Support for $countdrivers
      ಆಭಾಸ vvp_bit4_t driven_ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ;
      ಆಭಾಸ bool is_forced(unsigned idx) ಧ್ರುವ;
};

class vvp_wire_vec4 : ಸಾರ್ವಜನಿಕ vvp_wire_base {

    ಸಾರ್ವಜನಿಕ:
      vvp_wire_vec4(unsigned wid, vvp_bit4_t init);

	// The main filter behavior for this class. These methods take
	// the value that the node is driven to, and applies the force
	// filters. In wires, this also saves the driven value, so
	// that when a force is released, we can revert to the driven value.
      prop_t filter_vec4(ಧ್ರುವ vvp_vector4_t&bit, vvp_vector4_t&rep,
			 unsigned base, unsigned vwid) override;
      prop_t filter_vec8(ಧ್ರುವ vvp_vector8_t&val, vvp_vector8_t&rep,
			 unsigned base, unsigned vwid) override;

	// Abstract methods from vvp_vpi_callback
      ಶೂನ್ಯ get_ಮೌಲ್ಯ(ಸಂರಚನಾ t_vpi_ಮೌಲ್ಯ*ಮೌಲ್ಯ) override;
	// Abstract methods from vvp_net_fit_t
      unsigned filter_size() ಧ್ರುವ override;
      ಶೂನ್ಯ force_fil_vec4(ಧ್ರುವ vvp_vector4_t&val, ಧ್ರುವ vvp_vector2_t&mask) override;
      ಶೂನ್ಯ force_fil_vec8(ಧ್ರುವ vvp_vector8_t&val, ಧ್ರುವ vvp_vector2_t&mask) override;
      ಶೂನ್ಯ force_fil_real(ದ್ವಿಗುಣ val, ಧ್ರುವ vvp_vector2_t&mask) override;
      ಶೂನ್ಯ release(vvp_net_ptr_t ptr, bool net_flag) override;
      ಶೂನ್ಯ release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      unsigned ಮೌಲ್ಯ_size() ಧ್ರುವ override;
      vvp_bit4_t ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ override;
      vvp_scalar_t scalar_ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ override;
      ಶೂನ್ಯ vec4_ಮೌಲ್ಯ(vvp_vector4_t&) ಧ್ರುವ override;

        // Support for $countdrivers
      vvp_bit4_t driven_ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ override;
      bool is_forced(unsigned idx) ಧ್ರುವ override;

    ಗುಪ್ತ:
      vvp_bit4_t filtered_ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ;

    ಗುಪ್ತ:
      bool needs_init_;
      vvp_vector4_t bits4_; // The tracked driven value
      vvp_vector4_t force4_; // the value being forced
};

class vvp_wire_vec8 : ಸಾರ್ವಜನಿಕ vvp_wire_base {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_wire_vec8(unsigned wid);

	// The main filter behavior for this class
      prop_t filter_vec4(ಧ್ರುವ vvp_vector4_t&bit, vvp_vector4_t&rep,
			 unsigned base, unsigned vwid) override;
      prop_t filter_vec8(ಧ್ರುವ vvp_vector8_t&val, vvp_vector8_t&rep,
			 unsigned base, unsigned vwid) override;

	// island ports use this method to filter arbitrary values
	// through the force filter.
      prop_t filter_ಒಳಹರಿವು_vec8(ಧ್ರುವ vvp_vector8_t&val, vvp_vector8_t&rep) ಧ್ರುವ;


	// Abstract methods from vvp_vpi_callback
      ಶೂನ್ಯ get_ಮೌಲ್ಯ(ಸಂರಚನಾ t_vpi_ಮೌಲ್ಯ*ಮೌಲ್ಯ) override;
	// Abstract methods from vvp_net_fit_t
      unsigned filter_size() ಧ್ರುವ override;
      ಶೂನ್ಯ force_fil_vec4(ಧ್ರುವ vvp_vector4_t&val, ಧ್ರುವ vvp_vector2_t&mask) override;
      ಶೂನ್ಯ force_fil_vec8(ಧ್ರುವ vvp_vector8_t&val, ಧ್ರುವ vvp_vector2_t&mask) override;
      ಶೂನ್ಯ force_fil_real(ದ್ವಿಗುಣ val, ಧ್ರುವ vvp_vector2_t&mask) override;
      ಶೂನ್ಯ release(vvp_net_ptr_t ptr, bool net_flag) override;
      ಶೂನ್ಯ release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      unsigned ಮೌಲ್ಯ_size() ಧ್ರುವ override;
      vvp_bit4_t ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ override;
      vvp_scalar_t scalar_ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ override;
      ಶೂನ್ಯ vec4_ಮೌಲ್ಯ(vvp_vector4_t&) ಧ್ರುವ override;
	// This is new to vvp_wire_vec8
      vvp_vector8_t vec8_ಮೌಲ್ಯ() ಧ್ರುವ;

        // Support for $countdrivers
      vvp_bit4_t driven_ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ override;
      bool is_forced(unsigned idx) ಧ್ರುವ override;

    ಗುಪ್ತ:
      vvp_scalar_t filtered_ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ;

    ಗುಪ್ತ:
      bool needs_init_;
      vvp_vector8_t bits8_;
      vvp_vector8_t force8_; // the value being forced
};

class vvp_wire_real : ಸಾರ್ವಜನಿಕ vvp_wire_base {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_wire_real(ಶೂನ್ಯ);

	// The main filter behavior for this class
      prop_t filter_real(ದ್ವಿಗುಣ&bit) override;

	// Abstract methods from vvp_vpi_callback
      ಶೂನ್ಯ get_ಮೌಲ್ಯ(ಸಂರಚನಾ t_vpi_ಮೌಲ್ಯ*ಮೌಲ್ಯ) override;
	// Abstract methods from vvp_net_fit_t
      unsigned filter_size() ಧ್ರುವ override;
      ಶೂನ್ಯ force_fil_vec4(ಧ್ರುವ vvp_vector4_t&val, ಧ್ರುವ vvp_vector2_t&mask) override;
      ಶೂನ್ಯ force_fil_vec8(ಧ್ರುವ vvp_vector8_t&val, ಧ್ರುವ vvp_vector2_t&mask) override;
      ಶೂನ್ಯ force_fil_real(ದ್ವಿಗುಣ val, ಧ್ರುವ vvp_vector2_t&mask) override;
      ಶೂನ್ಯ release(vvp_net_ptr_t ptr, bool net_flag) override;
      ಶೂನ್ಯ release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      unsigned ಮೌಲ್ಯ_size() ಧ್ರುವ override;
      vvp_bit4_t ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ override;
      vvp_scalar_t scalar_ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ override;
      ಶೂನ್ಯ vec4_ಮೌಲ್ಯ(vvp_vector4_t&) ಧ್ರುವ override;
      ದ್ವಿಗುಣ real_ಮೌಲ್ಯ() ಧ್ರುವ override;

      ಶೂನ್ಯ get_signal_ಮೌಲ್ಯ(ಸಂರಚನಾ t_vpi_ಮೌಲ್ಯ*vp) override;

    ಗುಪ್ತ:
      ದ್ವಿಗುಣ bit_;
      ದ್ವಿಗುಣ force_;
      bool needs_init_ = true;
};

#ಯದಿ 0
class vvp_wire_string : ಸಾರ್ವಜನಿಕ vvp_wire_base {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_wire_string(ಶೂನ್ಯ);

	// Abstract methods from vvp_vpi_callback
      ಶೂನ್ಯ get_ಮೌಲ್ಯ(ಸಂರಚನಾ t_vpi_ಮೌಲ್ಯ*ಮೌಲ್ಯ);
	// Abstract methods from vvp_net_fil_t
      unsigned filter_size() ಧ್ರುವ;
      ಶೂನ್ಯ force_fil_vec4(ಧ್ರುವ vvp_vector4_t&val, ಧ್ರುವ vvp_vector2_t&mask);
      ಶೂನ್ಯ force_fil_vec8(ಧ್ರುವ vvp_vector8_t&val, ಧ್ರುವ vvp_vector2_t&mask);
      ಶೂನ್ಯ force_fil_real(ದ್ವಿಗುಣ val, ಧ್ರುವ vvp_vector2_t&mask);
      ಶೂನ್ಯ release(vvp_net_ptr_t ptr, bool net_flag);
      ಶೂನ್ಯ release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag);

	// Implementation of vvp_signal_value methods
      unsigned ಮೌಲ್ಯ_size() ಧ್ರುವ;
      vvp_bit4_t ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ;
      vvp_scalar_t scalar_ಮೌಲ್ಯ(unsigned idx) ಧ್ರುವ;
      ಶೂನ್ಯ vec4_ಮೌಲ್ಯ(vvp_vector4_t&) ಧ್ರುವ;
      ದ್ವಿಗುಣ real_ಮೌಲ್ಯ() ಧ್ರುವ;

      ಶೂನ್ಯ get_signal_ಮೌಲ್ಯ(ಸಂರಚನಾ t_vpi_ಮೌಲ್ಯ*vp);

    ಗುಪ್ತ:
      std::string ಮೌಲ್ಯ_2;
};
#endif

#endif /* IVL_vvp_net_sig_H */
