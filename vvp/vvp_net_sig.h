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
#অন্যথা
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


class vvp_fun_signal_base : সার্ৱজনিক vvp_net_fun_t {

    সার্ৱজনিক:
      vvp_fun_signal_base();

      শূন্য deassign();
      শূন্য deassign_pv(unsigned base, unsigned wid);

    সার্ৱজনিক:

	/* The %cassign/link instruction needs a place to write the
	   source node of the force, so that subsequent %cassign and
	   %deassign instructions can undo the link as needed. */
      class vvp_net_t*cassign_link;

    রক্ষিত:
      bool continuous_assign_active_;
      vvp_vector2_t assign_mask_;

    রক্ষিত:
	// This is true until at least one propagation happens.
      bool needs_init_;
};

/*
 * Variables and wires can have their values accessed, so this base
 * class offers the unified concept of an accessible value.
 */
class vvp_signal_মান {
    সার্ৱজনিক:
      আভাস ~vvp_signal_মান() =0;
      আভাস unsigned মান_size() ধ্রুৱ =0;
      আভাস vvp_bit4_t মান(unsigned idx) ধ্রুৱ =0;
      আভাস vvp_scalar_t scalar_মান(unsigned idx) ধ্রুৱ =0;
      আভাস শূন্য vec4_মান(vvp_vector4_t&) ধ্রুৱ =0;
      আভাস দ্ৱিগুণ real_মান() ধ্রুৱ;

      আভাস শূন্য get_signal_মান(সংরচনা t_vpi_মান*vp);
};

/*
 * This abstract class is a little more specific than the signal_base
 * class, in that it adds vector access methods.
 */
class vvp_fun_signal_vec : সার্ৱজনিক vvp_fun_signal_base {
    সার্ৱজনিক:
      আভাস ধ্রুৱ vvp_vector4_t& vec4_unfiltered_মান() ধ্রুৱ =0;
};

class automatic_signal_base : সার্ৱজনিক vvp_signal_মান, সার্ৱজনিক vvp_net_fil_t {

    সার্ৱজনিক:
	// Automatic variables cannot be forced or released. Provide
	// stubs that assert.
      আভাস শূন্য release(vvp_net_ptr_t ptr, bool net_flag) override;
      আভাস শূন্য release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

      আভাস unsigned filter_size() ধ্রুৱ override;
      আভাস শূন্য force_fil_vec4(ধ্রুৱ vvp_vector4_t&val, ধ্রুৱ vvp_vector2_t&mask) override;
      আভাস শূন্য force_fil_vec8(ধ্রুৱ vvp_vector8_t&val, ধ্রুৱ vvp_vector2_t&mask) override;
      আভাস শূন্য force_fil_real(দ্ৱিগুণ val, ধ্রুৱ vvp_vector2_t&mask) override;
      আভাস শূন্য get_মান(সংরচনা t_vpi_মান*মান) override;
};

/*
 * Statically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_sa : সার্ৱজনিক vvp_fun_signal_vec {

    সার্ৱজনিক:
      explicit vvp_fun_signal4_sa(unsigned wid, vvp_bit4_t init=BIT4_X);

      শূন্য recv_vec4(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
                     vvp_context_t) override;
      শূন্য recv_vec8(vvp_net_ptr_t port, ধ্রুৱ vvp_vector8_t&bit) override;

	// Part select variants of above
      শূন্য recv_vec4_pv(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t) override;
      শূন্য recv_vec8_pv(vvp_net_ptr_t port, ধ্রুৱ vvp_vector8_t&bit,
			unsigned base, unsigned vwid) override;

	// Get information about the vector value.
      ধ্রুৱ vvp_vector4_t& vec4_unfiltered_মান() ধ্রুৱ override;

    গুপ্ত:
      vvp_vector4_t bits4_;
};

/*
 * Automatically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_aa : সার্ৱজনিক vvp_fun_signal_vec, সার্ৱজনিক automatic_signal_base, সার্ৱজনিক automatic_hooks_s {

    সার্ৱজনিক:
      explicit vvp_fun_signal4_aa(unsigned wid, vvp_bit4_t init=BIT4_X);
      ~vvp_fun_signal4_aa() override;

      শূন্য alloc_instance(vvp_context_t context) override;
      শূন্য reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      শূন্য free_instance(vvp_context_t context) override;
#endif

      শূন্য recv_vec4(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
                     vvp_context_t context) override;

	// Part select variants of above
      শূন্য recv_vec4_pv(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t) override;

	// Get information about the vector value.
      unsigned   মান_size() ধ্রুৱ override;
      vvp_bit4_t মান(unsigned idx) ধ্রুৱ override;
      vvp_scalar_t scalar_মান(unsigned idx) ধ্রুৱ override;
      শূন্য vec4_মান(vvp_vector4_t&) ধ্রুৱ override;
      ধ্রুৱ vvp_vector4_t& vec4_unfiltered_মান() ধ্রুৱ override;

    সার্ৱজনিক: // These objects are only permallocated.
      স্থির শূন্য* operator new(std::size_t size) { ঘূৰাই vvp_net_fun_t::heap_.alloc(size); }
      স্থির শূন্য operator delete(শূন্য*obj);

    গুপ্ত:
      unsigned context_idx_;
      unsigned size_;
      vvp_bit4_t init_;
};

class vvp_fun_signal_real : সার্ৱজনিক vvp_fun_signal_base {

    সার্ৱজনিক:
      explicit vvp_fun_signal_real() {};

	// Get information about the vector value.
      আভাস দ্ৱিগুণ real_unfiltered_মান() ধ্রুৱ = 0;
};

/*
 * Statically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_sa : সার্ৱজনিক vvp_fun_signal_real {

    সার্ৱজনিক:
      explicit vvp_fun_signal_real_sa();

      শূন্য recv_real(vvp_net_ptr_t port, দ্ৱিগুণ bit,
                     vvp_context_t) override;

	// Get information about the vector value.
      দ্ৱিগুণ real_unfiltered_মান() ধ্রুৱ override;

    গুপ্ত:
      দ্ৱিগুণ bits_;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_aa : সার্ৱজনিক vvp_fun_signal_real, সার্ৱজনিক automatic_signal_base, সার্ৱজনিক automatic_hooks_s {

    সার্ৱজনিক:
      explicit vvp_fun_signal_real_aa();
      ~vvp_fun_signal_real_aa() override;

      শূন্য alloc_instance(vvp_context_t context) override;
      শূন্য reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      শূন্য free_instance(vvp_context_t context) override;
#endif

      শূন্য recv_real(vvp_net_ptr_t port, দ্ৱিগুণ bit,
                     vvp_context_t context) override;

	// Get information about the vector value.
      দ্ৱিগুণ real_unfiltered_মান() ধ্রুৱ override;

	// Get information about the vector value.
      unsigned   মান_size() ধ্রুৱ override;
      vvp_bit4_t মান(unsigned idx) ধ্রুৱ override;
      vvp_scalar_t scalar_মান(unsigned idx) ধ্রুৱ override;
      শূন্য vec4_মান(vvp_vector4_t&) ধ্রুৱ override;
      দ্ৱিগুণ real_মান() ধ্রুৱ override;
      শূন্য get_signal_মান(সংরচনা t_vpi_মান*vp) override;

    সার্ৱজনিক: // These objects are only permallocated.
      স্থির শূন্য* operator new(std::size_t size);
      স্থির শূন্য operator delete(শূন্য*obj);

    গুপ্ত:
      unsigned context_idx_;
};


class vvp_fun_signal_string : সার্ৱজনিক vvp_fun_signal_base {

    সার্ৱজনিক:
      explicit vvp_fun_signal_string() {};

      আভাস ধ্রুৱ std::string& get_string() ধ্রুৱ =0;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_string_sa : সার্ৱজনিক vvp_fun_signal_string {

    সার্ৱজনিক:
      explicit vvp_fun_signal_string_sa();

      শূন্য recv_string(vvp_net_ptr_t port, ধ্রুৱ std::string&bit,
		       vvp_context_t context) override;

      ধ্রুৱ std::string& get_string() ধ্রুৱ override;

    গুপ্ত:
      std::string মান_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_string_aa : সার্ৱজনিক vvp_fun_signal_string, সার্ৱজনিক automatic_signal_base, সার্ৱজনিক automatic_hooks_s {

    সার্ৱজনিক:
      explicit vvp_fun_signal_string_aa();
      ~vvp_fun_signal_string_aa() override;

      শূন্য alloc_instance(vvp_context_t context) override;
      শূন্য reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      শূন্য free_instance(vvp_context_t context) override;
#endif
      শূন্য recv_string(vvp_net_ptr_t port, ধ্রুৱ std::string&bit,
		       vvp_context_t context) override;

	// Get information about the vector value.
      unsigned   মান_size() ধ্রুৱ override;
      vvp_bit4_t মান(unsigned idx) ধ্রুৱ override;
      vvp_scalar_t scalar_মান(unsigned idx) ধ্রুৱ override;
      শূন্য vec4_মান(vvp_vector4_t&) ধ্রুৱ override;
      দ্ৱিগুণ real_মান() ধ্রুৱ override;
      ধ্রুৱ std::string& get_string() ধ্রুৱ override;
      শূন্য get_signal_মান(সংরচনা t_vpi_মান*vp) override;

    সার্ৱজনিক: // These objects are only permallocated.
      স্থির শূন্য* operator new(std::size_t size);
      স্থির শূন্য operator delete(শূন্য*obj);

    গুপ্ত:
      unsigned context_idx_;
};

class vvp_fun_signal_object : সার্ৱজনিক vvp_fun_signal_base {

    সার্ৱজনিক:
      explicit vvp_fun_signal_object(unsigned size) { size_ = size; };
      unsigned size() ধ্রুৱ { ঘূৰাই size_; }

      আভাস vvp_object_t get_object() ধ্রুৱ =0;
    গুপ্ত:
      unsigned size_;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_object_sa : সার্ৱজনিক vvp_fun_signal_object {

    সার্ৱজনিক:
      explicit vvp_fun_signal_object_sa(unsigned size);

      শূন্য recv_object(vvp_net_ptr_t port, vvp_object_t bit,
		    vvp_context_t context) override;

      vvp_object_t get_object() ধ্রুৱ override;

    গুপ্ত:
      vvp_object_t মান_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_object_aa : সার্ৱজনিক vvp_fun_signal_object, সার্ৱজনিক automatic_signal_base, সার্ৱজনিক automatic_hooks_s {

    সার্ৱজনিক:
      explicit vvp_fun_signal_object_aa(unsigned size);
      ~vvp_fun_signal_object_aa() override;

      শূন্য alloc_instance(vvp_context_t context) override;
      শূন্য reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      শূন্য free_instance(vvp_context_t context) override;
#endif

      শূন্য recv_object(vvp_net_ptr_t port, vvp_object_t bit,
		    vvp_context_t context) override;

	// Get information about the vector value.
      unsigned   মান_size() ধ্রুৱ override;
      vvp_bit4_t মান(unsigned idx) ধ্রুৱ override;
      vvp_scalar_t scalar_মান(unsigned idx) ধ্রুৱ override;
      শূন্য vec4_মান(vvp_vector4_t&) ধ্রুৱ override;
	//double real_value() const;
	//void get_signal_value(struct t_vpi_value*vp);

      vvp_object_t get_object() ধ্রুৱ override;

    সার্ৱজনিক: // These objects are only permallocated.
      স্থির শূন্য* operator new(std::size_t size);
      স্থির শূন্য operator delete(শূন্য*obj);

    গুপ্ত:
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

class vvp_wire_base  : সার্ৱজনিক vvp_net_fil_t, সার্ৱজনিক vvp_signal_মান {

    সার্ৱজনিক:
      vvp_wire_base();
      ~vvp_wire_base() override;

        // Support for $countdrivers
      আভাস vvp_bit4_t driven_মান(unsigned idx) ধ্রুৱ;
      আভাস bool is_forced(unsigned idx) ধ্রুৱ;
};

class vvp_wire_vec4 : সার্ৱজনিক vvp_wire_base {

    সার্ৱজনিক:
      vvp_wire_vec4(unsigned wid, vvp_bit4_t init);

	// The main filter behavior for this class. These methods take
	// the value that the node is driven to, and applies the force
	// filters. In wires, this also saves the driven value, so
	// that when a force is released, we can revert to the driven value.
      prop_t filter_vec4(ধ্রুৱ vvp_vector4_t&bit, vvp_vector4_t&rep,
			 unsigned base, unsigned vwid) override;
      prop_t filter_vec8(ধ্রুৱ vvp_vector8_t&val, vvp_vector8_t&rep,
			 unsigned base, unsigned vwid) override;

	// Abstract methods from vvp_vpi_callback
      শূন্য get_মান(সংরচনা t_vpi_মান*মান) override;
	// Abstract methods from vvp_net_fit_t
      unsigned filter_size() ধ্রুৱ override;
      শূন্য force_fil_vec4(ধ্রুৱ vvp_vector4_t&val, ধ্রুৱ vvp_vector2_t&mask) override;
      শূন্য force_fil_vec8(ধ্রুৱ vvp_vector8_t&val, ধ্রুৱ vvp_vector2_t&mask) override;
      শূন্য force_fil_real(দ্ৱিগুণ val, ধ্রুৱ vvp_vector2_t&mask) override;
      শূন্য release(vvp_net_ptr_t ptr, bool net_flag) override;
      শূন্য release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      unsigned মান_size() ধ্রুৱ override;
      vvp_bit4_t মান(unsigned idx) ধ্রুৱ override;
      vvp_scalar_t scalar_মান(unsigned idx) ধ্রুৱ override;
      শূন্য vec4_মান(vvp_vector4_t&) ধ্রুৱ override;

        // Support for $countdrivers
      vvp_bit4_t driven_মান(unsigned idx) ধ্রুৱ override;
      bool is_forced(unsigned idx) ধ্রুৱ override;

    গুপ্ত:
      vvp_bit4_t filtered_মান(unsigned idx) ধ্রুৱ;

    গুপ্ত:
      bool needs_init_;
      vvp_vector4_t bits4_; // The tracked driven value
      vvp_vector4_t force4_; // the value being forced
};

class vvp_wire_vec8 : সার্ৱজনিক vvp_wire_base {

    সার্ৱজনিক:
      explicit vvp_wire_vec8(unsigned wid);

	// The main filter behavior for this class
      prop_t filter_vec4(ধ্রুৱ vvp_vector4_t&bit, vvp_vector4_t&rep,
			 unsigned base, unsigned vwid) override;
      prop_t filter_vec8(ধ্রুৱ vvp_vector8_t&val, vvp_vector8_t&rep,
			 unsigned base, unsigned vwid) override;

	// island ports use this method to filter arbitrary values
	// through the force filter.
      prop_t filter_প্ৰবেশ_vec8(ধ্রুৱ vvp_vector8_t&val, vvp_vector8_t&rep) ধ্রুৱ;


	// Abstract methods from vvp_vpi_callback
      শূন্য get_মান(সংরচনা t_vpi_মান*মান) override;
	// Abstract methods from vvp_net_fit_t
      unsigned filter_size() ধ্রুৱ override;
      শূন্য force_fil_vec4(ধ্রুৱ vvp_vector4_t&val, ধ্রুৱ vvp_vector2_t&mask) override;
      শূন্য force_fil_vec8(ধ্রুৱ vvp_vector8_t&val, ধ্রুৱ vvp_vector2_t&mask) override;
      শূন্য force_fil_real(দ্ৱিগুণ val, ধ্রুৱ vvp_vector2_t&mask) override;
      শূন্য release(vvp_net_ptr_t ptr, bool net_flag) override;
      শূন্য release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      unsigned মান_size() ধ্রুৱ override;
      vvp_bit4_t মান(unsigned idx) ধ্রুৱ override;
      vvp_scalar_t scalar_মান(unsigned idx) ধ্রুৱ override;
      শূন্য vec4_মান(vvp_vector4_t&) ধ্রুৱ override;
	// This is new to vvp_wire_vec8
      vvp_vector8_t vec8_মান() ধ্রুৱ;

        // Support for $countdrivers
      vvp_bit4_t driven_মান(unsigned idx) ধ্রুৱ override;
      bool is_forced(unsigned idx) ধ্রুৱ override;

    গুপ্ত:
      vvp_scalar_t filtered_মান(unsigned idx) ধ্রুৱ;

    গুপ্ত:
      bool needs_init_;
      vvp_vector8_t bits8_;
      vvp_vector8_t force8_; // the value being forced
};

class vvp_wire_real : সার্ৱজনিক vvp_wire_base {

    সার্ৱজনিক:
      explicit vvp_wire_real(শূন্য);

	// The main filter behavior for this class
      prop_t filter_real(দ্ৱিগুণ&bit) override;

	// Abstract methods from vvp_vpi_callback
      শূন্য get_মান(সংরচনা t_vpi_মান*মান) override;
	// Abstract methods from vvp_net_fit_t
      unsigned filter_size() ধ্রুৱ override;
      শূন্য force_fil_vec4(ধ্রুৱ vvp_vector4_t&val, ধ্রুৱ vvp_vector2_t&mask) override;
      শূন্য force_fil_vec8(ধ্রুৱ vvp_vector8_t&val, ধ্রুৱ vvp_vector2_t&mask) override;
      শূন্য force_fil_real(দ্ৱিগুণ val, ধ্রুৱ vvp_vector2_t&mask) override;
      শূন্য release(vvp_net_ptr_t ptr, bool net_flag) override;
      শূন্য release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      unsigned মান_size() ধ্রুৱ override;
      vvp_bit4_t মান(unsigned idx) ধ্রুৱ override;
      vvp_scalar_t scalar_মান(unsigned idx) ধ্রুৱ override;
      শূন্য vec4_মান(vvp_vector4_t&) ধ্রুৱ override;
      দ্ৱিগুণ real_মান() ধ্রুৱ override;

      শূন্য get_signal_মান(সংরচনা t_vpi_মান*vp) override;

    গুপ্ত:
      দ্ৱিগুণ bit_;
      দ্ৱিগুণ force_;
      bool needs_init_ = true;
};

#যদি 0
class vvp_wire_string : সার্ৱজনিক vvp_wire_base {

    সার্ৱজনিক:
      explicit vvp_wire_string(শূন্য);

	// Abstract methods from vvp_vpi_callback
      শূন্য get_মান(সংরচনা t_vpi_মান*মান);
	// Abstract methods from vvp_net_fil_t
      unsigned filter_size() ধ্রুৱ;
      শূন্য force_fil_vec4(ধ্রুৱ vvp_vector4_t&val, ধ্রুৱ vvp_vector2_t&mask);
      শূন্য force_fil_vec8(ধ্রুৱ vvp_vector8_t&val, ধ্রুৱ vvp_vector2_t&mask);
      শূন্য force_fil_real(দ্ৱিগুণ val, ধ্রুৱ vvp_vector2_t&mask);
      শূন্য release(vvp_net_ptr_t ptr, bool net_flag);
      শূন্য release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag);

	// Implementation of vvp_signal_value methods
      unsigned মান_size() ধ্রুৱ;
      vvp_bit4_t মান(unsigned idx) ধ্রুৱ;
      vvp_scalar_t scalar_মান(unsigned idx) ধ্রুৱ;
      শূন্য vec4_মান(vvp_vector4_t&) ধ্রুৱ;
      দ্ৱিগুণ real_মান() ধ্রুৱ;

      শূন্য get_signal_মান(সংরচনা t_vpi_মান*vp);

    গুপ্ত:
      std::string মান_2;
};
#endif

#endif /* IVL_vvp_net_sig_H */
