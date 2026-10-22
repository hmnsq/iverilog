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


class vvp_fun_signal_base : সর্বজনীন vvp_net_fun_t {

    সর্বজনীন:
      vvp_fun_signal_base();

      শূন্য deassign();
      শূন্য deassign_pv(unsigned base, unsigned wid);

    সর্বজনীন:

	/* The %cassign/link instruction needs a place to write the
	   source node of the force, so that subsequent %cassign and
	   %deassign instructions can undo the link as needed. */
      class vvp_net_t*cassign_link;

    সুরক্ষিত:
      bool continuous_assign_active_;
      vvp_vector2_t assign_mask_;

    সুরক্ষিত:
	// This is true until at least one propagation happens.
      bool needs_init_;
};

/*
 * Variables and wires can have their values accessed, so this base
 * class offers the unified concept of an accessible value.
 */
class vvp_signal_মান {
    সর্বজনীন:
      আভাসী ~vvp_signal_মান() =0;
      আভাসী unsigned মান_আকার() ধ্রুবক =0;
      আভাসী vvp_bit4_t মান(unsigned idx) ধ্রুবক =0;
      আভাসী vvp_scalar_t scalar_মান(unsigned idx) ধ্রুবক =0;
      আভাসী শূন্য vec4_মান(vvp_vector4_t&) ধ্রুবক =0;
      আভাসী দ্বিগুণ real_মান() ধ্রুবক;

      আভাসী শূন্য get_signal_মান(কাঠামো t_vpi_মান*vp);
};

/*
 * This abstract class is a little more specific than the signal_base
 * class, in that it adds vector access methods.
 */
class vvp_fun_signal_vec : সর্বজনীন vvp_fun_signal_base {
    সর্বজনীন:
      আভাসী ধ্রুবক vvp_vector4_t& vec4_unfiltered_মান() ধ্রুবক =0;
};

class automatic_signal_base : সর্বজনীন vvp_signal_মান, সর্বজনীন vvp_net_fil_t {

    সর্বজনীন:
	// Automatic variables cannot be forced or released. Provide
	// stubs that assert.
      আভাসী শূন্য release(vvp_net_ptr_t ptr, bool net_flag) override;
      আভাসী শূন্য release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

      আভাসী unsigned filter_আকার() ধ্রুবক override;
      আভাসী শূন্য force_fil_vec4(ধ্রুবক vvp_vector4_t&val, ধ্রুবক vvp_vector2_t&mask) override;
      আভাসী শূন্য force_fil_vec8(ধ্রুবক vvp_vector8_t&val, ধ্রুবক vvp_vector2_t&mask) override;
      আভাসী শূন্য force_fil_real(দ্বিগুণ val, ধ্রুবক vvp_vector2_t&mask) override;
      আভাসী শূন্য get_মান(কাঠামো t_vpi_মান*মান) override;
};

/*
 * Statically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_sa : সর্বজনীন vvp_fun_signal_vec {

    সর্বজনীন:
      explicit vvp_fun_signal4_sa(unsigned wid, vvp_bit4_t init=BIT4_X);

      শূন্য recv_vec4(vvp_net_ptr_t port, ধ্রুবক vvp_vector4_t&bit,
                     vvp_context_t) override;
      শূন্য recv_vec8(vvp_net_ptr_t port, ধ্রুবক vvp_vector8_t&bit) override;

	// Part select variants of above
      শূন্য recv_vec4_pv(vvp_net_ptr_t port, ধ্রুবক vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t) override;
      শূন্য recv_vec8_pv(vvp_net_ptr_t port, ধ্রুবক vvp_vector8_t&bit,
			unsigned base, unsigned vwid) override;

	// Get information about the vector value.
      ধ্রুবক vvp_vector4_t& vec4_unfiltered_মান() ধ্রুবক override;

    ব্যক্তিগত:
      vvp_vector4_t bits4_;
};

/*
 * Automatically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_aa : সর্বজনীন vvp_fun_signal_vec, সর্বজনীন automatic_signal_base, সর্বজনীন automatic_hooks_s {

    সর্বজনীন:
      explicit vvp_fun_signal4_aa(unsigned wid, vvp_bit4_t init=BIT4_X);
      ~vvp_fun_signal4_aa() override;

      শূন্য alloc_instance(vvp_context_t context) override;
      শূন্য reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      শূন্য free_instance(vvp_context_t context) override;
#endif

      শূন্য recv_vec4(vvp_net_ptr_t port, ধ্রুবক vvp_vector4_t&bit,
                     vvp_context_t context) override;

	// Part select variants of above
      শূন্য recv_vec4_pv(vvp_net_ptr_t port, ধ্রুবক vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t) override;

	// Get information about the vector value.
      unsigned   মান_আকার() ধ্রুবক override;
      vvp_bit4_t মান(unsigned idx) ধ্রুবক override;
      vvp_scalar_t scalar_মান(unsigned idx) ধ্রুবক override;
      শূন্য vec4_মান(vvp_vector4_t&) ধ্রুবক override;
      ধ্রুবক vvp_vector4_t& vec4_unfiltered_মান() ধ্রুবক override;

    সর্বজনীন: // These objects are only permallocated.
      স্থির শূন্য* operator new(std::size_t আকার_2) { ফেরত vvp_net_fun_t::heap_.alloc(আকার_2); }
      স্থির শূন্য operator delete(শূন্য*obj);

    ব্যক্তিগত:
      unsigned context_idx_;
      unsigned আকার_3;
      vvp_bit4_t init_;
};

class vvp_fun_signal_real : সর্বজনীন vvp_fun_signal_base {

    সর্বজনীন:
      explicit vvp_fun_signal_real() {};

	// Get information about the vector value.
      আভাসী দ্বিগুণ real_unfiltered_মান() ধ্রুবক = 0;
};

/*
 * Statically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_sa : সর্বজনীন vvp_fun_signal_real {

    সর্বজনীন:
      explicit vvp_fun_signal_real_sa();

      শূন্য recv_real(vvp_net_ptr_t port, দ্বিগুণ bit,
                     vvp_context_t) override;

	// Get information about the vector value.
      দ্বিগুণ real_unfiltered_মান() ধ্রুবক override;

    ব্যক্তিগত:
      দ্বিগুণ bits_;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_aa : সর্বজনীন vvp_fun_signal_real, সর্বজনীন automatic_signal_base, সর্বজনীন automatic_hooks_s {

    সর্বজনীন:
      explicit vvp_fun_signal_real_aa();
      ~vvp_fun_signal_real_aa() override;

      শূন্য alloc_instance(vvp_context_t context) override;
      শূন্য reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      শূন্য free_instance(vvp_context_t context) override;
#endif

      শূন্য recv_real(vvp_net_ptr_t port, দ্বিগুণ bit,
                     vvp_context_t context) override;

	// Get information about the vector value.
      দ্বিগুণ real_unfiltered_মান() ধ্রুবক override;

	// Get information about the vector value.
      unsigned   মান_আকার() ধ্রুবক override;
      vvp_bit4_t মান(unsigned idx) ধ্রুবক override;
      vvp_scalar_t scalar_মান(unsigned idx) ধ্রুবক override;
      শূন্য vec4_মান(vvp_vector4_t&) ধ্রুবক override;
      দ্বিগুণ real_মান() ধ্রুবক override;
      শূন্য get_signal_মান(কাঠামো t_vpi_মান*vp) override;

    সর্বজনীন: // These objects are only permallocated.
      স্থির শূন্য* operator new(std::size_t আকার_2);
      স্থির শূন্য operator delete(শূন্য*obj);

    ব্যক্তিগত:
      unsigned context_idx_;
};


class vvp_fun_signal_string : সর্বজনীন vvp_fun_signal_base {

    সর্বজনীন:
      explicit vvp_fun_signal_string() {};

      আভাসী ধ্রুবক std::string& get_string() ধ্রুবক =0;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_string_sa : সর্বজনীন vvp_fun_signal_string {

    সর্বজনীন:
      explicit vvp_fun_signal_string_sa();

      শূন্য recv_string(vvp_net_ptr_t port, ধ্রুবক std::string&bit,
		       vvp_context_t context) override;

      ধ্রুবক std::string& get_string() ধ্রুবক override;

    ব্যক্তিগত:
      std::string মান_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_string_aa : সর্বজনীন vvp_fun_signal_string, সর্বজনীন automatic_signal_base, সর্বজনীন automatic_hooks_s {

    সর্বজনীন:
      explicit vvp_fun_signal_string_aa();
      ~vvp_fun_signal_string_aa() override;

      শূন্য alloc_instance(vvp_context_t context) override;
      শূন্য reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      শূন্য free_instance(vvp_context_t context) override;
#endif
      শূন্য recv_string(vvp_net_ptr_t port, ধ্রুবক std::string&bit,
		       vvp_context_t context) override;

	// Get information about the vector value.
      unsigned   মান_আকার() ধ্রুবক override;
      vvp_bit4_t মান(unsigned idx) ধ্রুবক override;
      vvp_scalar_t scalar_মান(unsigned idx) ধ্রুবক override;
      শূন্য vec4_মান(vvp_vector4_t&) ধ্রুবক override;
      দ্বিগুণ real_মান() ধ্রুবক override;
      ধ্রুবক std::string& get_string() ধ্রুবক override;
      শূন্য get_signal_মান(কাঠামো t_vpi_মান*vp) override;

    সর্বজনীন: // These objects are only permallocated.
      স্থির শূন্য* operator new(std::size_t আকার_2);
      স্থির শূন্য operator delete(শূন্য*obj);

    ব্যক্তিগত:
      unsigned context_idx_;
};

class vvp_fun_signal_object : সর্বজনীন vvp_fun_signal_base {

    সর্বজনীন:
      explicit vvp_fun_signal_object(unsigned আকার_2) { আকার_3 = আকার_2; };
      unsigned আকার_2() ধ্রুবক { ফেরত আকার_3; }

      আভাসী vvp_object_t get_object() ধ্রুবক =0;
    ব্যক্তিগত:
      unsigned আকার_3;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_object_sa : সর্বজনীন vvp_fun_signal_object {

    সর্বজনীন:
      explicit vvp_fun_signal_object_sa(unsigned আকার_2);

      শূন্য recv_object(vvp_net_ptr_t port, vvp_object_t bit,
		    vvp_context_t context) override;

      vvp_object_t get_object() ধ্রুবক override;

    ব্যক্তিগত:
      vvp_object_t মান_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_object_aa : সর্বজনীন vvp_fun_signal_object, সর্বজনীন automatic_signal_base, সর্বজনীন automatic_hooks_s {

    সর্বজনীন:
      explicit vvp_fun_signal_object_aa(unsigned আকার_2);
      ~vvp_fun_signal_object_aa() override;

      শূন্য alloc_instance(vvp_context_t context) override;
      শূন্য reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      শূন্য free_instance(vvp_context_t context) override;
#endif

      শূন্য recv_object(vvp_net_ptr_t port, vvp_object_t bit,
		    vvp_context_t context) override;

	// Get information about the vector value.
      unsigned   মান_আকার() ধ্রুবক override;
      vvp_bit4_t মান(unsigned idx) ধ্রুবক override;
      vvp_scalar_t scalar_মান(unsigned idx) ধ্রুবক override;
      শূন্য vec4_মান(vvp_vector4_t&) ধ্রুবক override;
	//double real_value() const;
	//void get_signal_value(struct t_vpi_value*vp);

      vvp_object_t get_object() ধ্রুবক override;

    সর্বজনীন: // These objects are only permallocated.
      স্থির শূন্য* operator new(std::size_t আকার_2);
      স্থির শূন্য operator delete(শূন্য*obj);

    ব্যক্তিগত:
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

class vvp_wire_base  : সর্বজনীন vvp_net_fil_t, সর্বজনীন vvp_signal_মান {

    সর্বজনীন:
      vvp_wire_base();
      ~vvp_wire_base() override;

        // Support for $countdrivers
      আভাসী vvp_bit4_t driven_মান(unsigned idx) ধ্রুবক;
      আভাসী bool is_forced(unsigned idx) ধ্রুবক;
};

class vvp_wire_vec4 : সর্বজনীন vvp_wire_base {

    সর্বজনীন:
      vvp_wire_vec4(unsigned wid, vvp_bit4_t init);

	// The main filter behavior for this class. These methods take
	// the value that the node is driven to, and applies the force
	// filters. In wires, this also saves the driven value, so
	// that when a force is released, we can revert to the driven value.
      prop_t filter_vec4(ধ্রুবক vvp_vector4_t&bit, vvp_vector4_t&rep,
			 unsigned base, unsigned vwid) override;
      prop_t filter_vec8(ধ্রুবক vvp_vector8_t&val, vvp_vector8_t&rep,
			 unsigned base, unsigned vwid) override;

	// Abstract methods from vvp_vpi_callback
      শূন্য get_মান(কাঠামো t_vpi_মান*মান) override;
	// Abstract methods from vvp_net_fit_t
      unsigned filter_আকার() ধ্রুবক override;
      শূন্য force_fil_vec4(ধ্রুবক vvp_vector4_t&val, ধ্রুবক vvp_vector2_t&mask) override;
      শূন্য force_fil_vec8(ধ্রুবক vvp_vector8_t&val, ধ্রুবক vvp_vector2_t&mask) override;
      শূন্য force_fil_real(দ্বিগুণ val, ধ্রুবক vvp_vector2_t&mask) override;
      শূন্য release(vvp_net_ptr_t ptr, bool net_flag) override;
      শূন্য release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      unsigned মান_আকার() ধ্রুবক override;
      vvp_bit4_t মান(unsigned idx) ধ্রুবক override;
      vvp_scalar_t scalar_মান(unsigned idx) ধ্রুবক override;
      শূন্য vec4_মান(vvp_vector4_t&) ধ্রুবক override;

        // Support for $countdrivers
      vvp_bit4_t driven_মান(unsigned idx) ধ্রুবক override;
      bool is_forced(unsigned idx) ধ্রুবক override;

    ব্যক্তিগত:
      vvp_bit4_t filtered_মান(unsigned idx) ধ্রুবক;

    ব্যক্তিগত:
      bool needs_init_;
      vvp_vector4_t bits4_; // The tracked driven value
      vvp_vector4_t force4_; // the value being forced
};

class vvp_wire_vec8 : সর্বজনীন vvp_wire_base {

    সর্বজনীন:
      explicit vvp_wire_vec8(unsigned wid);

	// The main filter behavior for this class
      prop_t filter_vec4(ধ্রুবক vvp_vector4_t&bit, vvp_vector4_t&rep,
			 unsigned base, unsigned vwid) override;
      prop_t filter_vec8(ধ্রুবক vvp_vector8_t&val, vvp_vector8_t&rep,
			 unsigned base, unsigned vwid) override;

	// island ports use this method to filter arbitrary values
	// through the force filter.
      prop_t filter_প্রবেশ_vec8(ধ্রুবক vvp_vector8_t&val, vvp_vector8_t&rep) ধ্রুবক;


	// Abstract methods from vvp_vpi_callback
      শূন্য get_মান(কাঠামো t_vpi_মান*মান) override;
	// Abstract methods from vvp_net_fit_t
      unsigned filter_আকার() ধ্রুবক override;
      শূন্য force_fil_vec4(ধ্রুবক vvp_vector4_t&val, ধ্রুবক vvp_vector2_t&mask) override;
      শূন্য force_fil_vec8(ধ্রুবক vvp_vector8_t&val, ধ্রুবক vvp_vector2_t&mask) override;
      শূন্য force_fil_real(দ্বিগুণ val, ধ্রুবক vvp_vector2_t&mask) override;
      শূন্য release(vvp_net_ptr_t ptr, bool net_flag) override;
      শূন্য release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      unsigned মান_আকার() ধ্রুবক override;
      vvp_bit4_t মান(unsigned idx) ধ্রুবক override;
      vvp_scalar_t scalar_মান(unsigned idx) ধ্রুবক override;
      শূন্য vec4_মান(vvp_vector4_t&) ধ্রুবক override;
	// This is new to vvp_wire_vec8
      vvp_vector8_t vec8_মান() ধ্রুবক;

        // Support for $countdrivers
      vvp_bit4_t driven_মান(unsigned idx) ধ্রুবক override;
      bool is_forced(unsigned idx) ধ্রুবক override;

    ব্যক্তিগত:
      vvp_scalar_t filtered_মান(unsigned idx) ধ্রুবক;

    ব্যক্তিগত:
      bool needs_init_;
      vvp_vector8_t bits8_;
      vvp_vector8_t force8_; // the value being forced
};

class vvp_wire_real : সর্বজনীন vvp_wire_base {

    সর্বজনীন:
      explicit vvp_wire_real(শূন্য);

	// The main filter behavior for this class
      prop_t filter_real(দ্বিগুণ&bit) override;

	// Abstract methods from vvp_vpi_callback
      শূন্য get_মান(কাঠামো t_vpi_মান*মান) override;
	// Abstract methods from vvp_net_fit_t
      unsigned filter_আকার() ধ্রুবক override;
      শূন্য force_fil_vec4(ধ্রুবক vvp_vector4_t&val, ধ্রুবক vvp_vector2_t&mask) override;
      শূন্য force_fil_vec8(ধ্রুবক vvp_vector8_t&val, ধ্রুবক vvp_vector2_t&mask) override;
      শূন্য force_fil_real(দ্বিগুণ val, ধ্রুবক vvp_vector2_t&mask) override;
      শূন্য release(vvp_net_ptr_t ptr, bool net_flag) override;
      শূন্য release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      unsigned মান_আকার() ধ্রুবক override;
      vvp_bit4_t মান(unsigned idx) ধ্রুবক override;
      vvp_scalar_t scalar_মান(unsigned idx) ধ্রুবক override;
      শূন্য vec4_মান(vvp_vector4_t&) ধ্রুবক override;
      দ্বিগুণ real_মান() ধ্রুবক override;

      শূন্য get_signal_মান(কাঠামো t_vpi_মান*vp) override;

    ব্যক্তিগত:
      দ্বিগুণ bit_;
      দ্বিগুণ force_;
      bool needs_init_ = true;
};

#যদি 0
class vvp_wire_string : সর্বজনীন vvp_wire_base {

    সর্বজনীন:
      explicit vvp_wire_string(শূন্য);

	// Abstract methods from vvp_vpi_callback
      শূন্য get_মান(কাঠামো t_vpi_মান*মান);
	// Abstract methods from vvp_net_fil_t
      unsigned filter_আকার() ধ্রুবক;
      শূন্য force_fil_vec4(ধ্রুবক vvp_vector4_t&val, ধ্রুবক vvp_vector2_t&mask);
      শূন্য force_fil_vec8(ধ্রুবক vvp_vector8_t&val, ধ্রুবক vvp_vector2_t&mask);
      শূন্য force_fil_real(দ্বিগুণ val, ধ্রুবক vvp_vector2_t&mask);
      শূন্য release(vvp_net_ptr_t ptr, bool net_flag);
      শূন্য release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag);

	// Implementation of vvp_signal_value methods
      unsigned মান_আকার() ধ্রুবক;
      vvp_bit4_t মান(unsigned idx) ধ্রুবক;
      vvp_scalar_t scalar_মান(unsigned idx) ধ্রুবক;
      শূন্য vec4_মান(vvp_vector4_t&) ধ্রুবক;
      দ্বিগুণ real_মান() ধ্রুবক;

      শূন্য get_signal_মান(কাঠামো t_vpi_মান*vp);

    ব্যক্তিগত:
      std::string মান_2;
};
#endif

#endif /* IVL_vvp_net_sig_H */
