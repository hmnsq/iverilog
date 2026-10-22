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
#وگرنه
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


class vvp_fun_signal_base : عمومی vvp_net_fun_t {

    عمومی:
      vvp_fun_signal_base();

      تهی deassign();
      تهی deassign_pv(unsigned base, unsigned wid);

    عمومی:

	/* The %cassign/link instruction needs a place to write the
	   source node of the force, so that subsequent %cassign and
	   %deassign instructions can undo the link as needed. */
      class vvp_net_t*cassign_link;

    محافظتشده:
      bool continuous_assign_active_;
      vvp_vector2_t assign_mask_;

    محافظتشده:
	// This is true until at least one propagation happens.
      bool needs_init_;
};

/*
 * Variables and wires can have their values accessed, so this base
 * class offers the unified concept of an accessible value.
 */
class vvp_signal_مقدار {
    عمومی:
      مجازی ~vvp_signal_مقدار() =0;
      مجازی unsigned مقدار_size() ثابت =0;
      مجازی vvp_bit4_t مقدار(unsigned idx) ثابت =0;
      مجازی vvp_scalar_t scalar_مقدار(unsigned idx) ثابت =0;
      مجازی تهی vec4_مقدار(vvp_vector4_t&) ثابت =0;
      مجازی دوگانه real_مقدار() ثابت;

      مجازی تهی get_signal_مقدار(ساختار t_vpi_مقدار*vp);
};

/*
 * This abstract class is a little more specific than the signal_base
 * class, in that it adds vector access methods.
 */
class vvp_fun_signal_vec : عمومی vvp_fun_signal_base {
    عمومی:
      مجازی ثابت vvp_vector4_t& vec4_unfiltered_مقدار() ثابت =0;
};

class automatic_signal_base : عمومی vvp_signal_مقدار, عمومی vvp_net_fil_t {

    عمومی:
	// Automatic variables cannot be forced or released. Provide
	// stubs that assert.
      مجازی تهی release(vvp_net_ptr_t ptr, bool net_flag) override;
      مجازی تهی release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

      مجازی unsigned filter_size() ثابت override;
      مجازی تهی force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&mask) override;
      مجازی تهی force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&mask) override;
      مجازی تهی force_fil_real(دوگانه val, ثابت vvp_vector2_t&mask) override;
      مجازی تهی get_مقدار(ساختار t_vpi_مقدار*مقدار) override;
};

/*
 * Statically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_sa : عمومی vvp_fun_signal_vec {

    عمومی:
      explicit vvp_fun_signal4_sa(unsigned wid, vvp_bit4_t init=BIT4_X);

      تهی recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                     vvp_context_t) override;
      تهی recv_vec8(vvp_net_ptr_t port, ثابت vvp_vector8_t&bit) override;

	// Part select variants of above
      تهی recv_vec4_pv(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t) override;
      تهی recv_vec8_pv(vvp_net_ptr_t port, ثابت vvp_vector8_t&bit,
			unsigned base, unsigned vwid) override;

	// Get information about the vector value.
      ثابت vvp_vector4_t& vec4_unfiltered_مقدار() ثابت override;

    خصوصی:
      vvp_vector4_t bits4_;
};

/*
 * Automatically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_aa : عمومی vvp_fun_signal_vec, عمومی automatic_signal_base, عمومی automatic_hooks_s {

    عمومی:
      explicit vvp_fun_signal4_aa(unsigned wid, vvp_bit4_t init=BIT4_X);
      ~vvp_fun_signal4_aa() override;

      تهی alloc_instance(vvp_context_t context) override;
      تهی reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      تهی free_instance(vvp_context_t context) override;
#endif

      تهی recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                     vvp_context_t context) override;

	// Part select variants of above
      تهی recv_vec4_pv(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t) override;

	// Get information about the vector value.
      unsigned   مقدار_size() ثابت override;
      vvp_bit4_t مقدار(unsigned idx) ثابت override;
      vvp_scalar_t scalar_مقدار(unsigned idx) ثابت override;
      تهی vec4_مقدار(vvp_vector4_t&) ثابت override;
      ثابت vvp_vector4_t& vec4_unfiltered_مقدار() ثابت override;

    عمومی: // These objects are only permallocated.
      ایستا تهی* operator new(std::size_t size) { بازگشت vvp_net_fun_t::heap_.alloc(size); }
      ایستا تهی operator delete(تهی*obj);

    خصوصی:
      unsigned context_idx_;
      unsigned size_;
      vvp_bit4_t init_;
};

class vvp_fun_signal_real : عمومی vvp_fun_signal_base {

    عمومی:
      explicit vvp_fun_signal_real() {};

	// Get information about the vector value.
      مجازی دوگانه real_unfiltered_مقدار() ثابت = 0;
};

/*
 * Statically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_sa : عمومی vvp_fun_signal_real {

    عمومی:
      explicit vvp_fun_signal_real_sa();

      تهی recv_real(vvp_net_ptr_t port, دوگانه bit,
                     vvp_context_t) override;

	// Get information about the vector value.
      دوگانه real_unfiltered_مقدار() ثابت override;

    خصوصی:
      دوگانه bits_;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_aa : عمومی vvp_fun_signal_real, عمومی automatic_signal_base, عمومی automatic_hooks_s {

    عمومی:
      explicit vvp_fun_signal_real_aa();
      ~vvp_fun_signal_real_aa() override;

      تهی alloc_instance(vvp_context_t context) override;
      تهی reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      تهی free_instance(vvp_context_t context) override;
#endif

      تهی recv_real(vvp_net_ptr_t port, دوگانه bit,
                     vvp_context_t context) override;

	// Get information about the vector value.
      دوگانه real_unfiltered_مقدار() ثابت override;

	// Get information about the vector value.
      unsigned   مقدار_size() ثابت override;
      vvp_bit4_t مقدار(unsigned idx) ثابت override;
      vvp_scalar_t scalar_مقدار(unsigned idx) ثابت override;
      تهی vec4_مقدار(vvp_vector4_t&) ثابت override;
      دوگانه real_مقدار() ثابت override;
      تهی get_signal_مقدار(ساختار t_vpi_مقدار*vp) override;

    عمومی: // These objects are only permallocated.
      ایستا تهی* operator new(std::size_t size);
      ایستا تهی operator delete(تهی*obj);

    خصوصی:
      unsigned context_idx_;
};


class vvp_fun_signal_string : عمومی vvp_fun_signal_base {

    عمومی:
      explicit vvp_fun_signal_string() {};

      مجازی ثابت std::string& get_string() ثابت =0;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_string_sa : عمومی vvp_fun_signal_string {

    عمومی:
      explicit vvp_fun_signal_string_sa();

      تهی recv_string(vvp_net_ptr_t port, ثابت std::string&bit,
		       vvp_context_t context) override;

      ثابت std::string& get_string() ثابت override;

    خصوصی:
      std::string مقدار_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_string_aa : عمومی vvp_fun_signal_string, عمومی automatic_signal_base, عمومی automatic_hooks_s {

    عمومی:
      explicit vvp_fun_signal_string_aa();
      ~vvp_fun_signal_string_aa() override;

      تهی alloc_instance(vvp_context_t context) override;
      تهی reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      تهی free_instance(vvp_context_t context) override;
#endif
      تهی recv_string(vvp_net_ptr_t port, ثابت std::string&bit,
		       vvp_context_t context) override;

	// Get information about the vector value.
      unsigned   مقدار_size() ثابت override;
      vvp_bit4_t مقدار(unsigned idx) ثابت override;
      vvp_scalar_t scalar_مقدار(unsigned idx) ثابت override;
      تهی vec4_مقدار(vvp_vector4_t&) ثابت override;
      دوگانه real_مقدار() ثابت override;
      ثابت std::string& get_string() ثابت override;
      تهی get_signal_مقدار(ساختار t_vpi_مقدار*vp) override;

    عمومی: // These objects are only permallocated.
      ایستا تهی* operator new(std::size_t size);
      ایستا تهی operator delete(تهی*obj);

    خصوصی:
      unsigned context_idx_;
};

class vvp_fun_signal_object : عمومی vvp_fun_signal_base {

    عمومی:
      explicit vvp_fun_signal_object(unsigned size) { size_ = size; };
      unsigned size() ثابت { بازگشت size_; }

      مجازی vvp_object_t get_object() ثابت =0;
    خصوصی:
      unsigned size_;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_object_sa : عمومی vvp_fun_signal_object {

    عمومی:
      explicit vvp_fun_signal_object_sa(unsigned size);

      تهی recv_object(vvp_net_ptr_t port, vvp_object_t bit,
		    vvp_context_t context) override;

      vvp_object_t get_object() ثابت override;

    خصوصی:
      vvp_object_t مقدار_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_object_aa : عمومی vvp_fun_signal_object, عمومی automatic_signal_base, عمومی automatic_hooks_s {

    عمومی:
      explicit vvp_fun_signal_object_aa(unsigned size);
      ~vvp_fun_signal_object_aa() override;

      تهی alloc_instance(vvp_context_t context) override;
      تهی reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      تهی free_instance(vvp_context_t context) override;
#endif

      تهی recv_object(vvp_net_ptr_t port, vvp_object_t bit,
		    vvp_context_t context) override;

	// Get information about the vector value.
      unsigned   مقدار_size() ثابت override;
      vvp_bit4_t مقدار(unsigned idx) ثابت override;
      vvp_scalar_t scalar_مقدار(unsigned idx) ثابت override;
      تهی vec4_مقدار(vvp_vector4_t&) ثابت override;
	//double real_value() const;
	//void get_signal_value(struct t_vpi_value*vp);

      vvp_object_t get_object() ثابت override;

    عمومی: // These objects are only permallocated.
      ایستا تهی* operator new(std::size_t size);
      ایستا تهی operator delete(تهی*obj);

    خصوصی:
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

class vvp_wire_base  : عمومی vvp_net_fil_t, عمومی vvp_signal_مقدار {

    عمومی:
      vvp_wire_base();
      ~vvp_wire_base() override;

        // Support for $countdrivers
      مجازی vvp_bit4_t driven_مقدار(unsigned idx) ثابت;
      مجازی bool is_forced(unsigned idx) ثابت;
};

class vvp_wire_vec4 : عمومی vvp_wire_base {

    عمومی:
      vvp_wire_vec4(unsigned wid, vvp_bit4_t init);

	// The main filter behavior for this class. These methods take
	// the value that the node is driven to, and applies the force
	// filters. In wires, this also saves the driven value, so
	// that when a force is released, we can revert to the driven value.
      prop_t filter_vec4(ثابت vvp_vector4_t&bit, vvp_vector4_t&rep,
			 unsigned base, unsigned vwid) override;
      prop_t filter_vec8(ثابت vvp_vector8_t&val, vvp_vector8_t&rep,
			 unsigned base, unsigned vwid) override;

	// Abstract methods from vvp_vpi_callback
      تهی get_مقدار(ساختار t_vpi_مقدار*مقدار) override;
	// Abstract methods from vvp_net_fit_t
      unsigned filter_size() ثابت override;
      تهی force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&mask) override;
      تهی force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&mask) override;
      تهی force_fil_real(دوگانه val, ثابت vvp_vector2_t&mask) override;
      تهی release(vvp_net_ptr_t ptr, bool net_flag) override;
      تهی release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      unsigned مقدار_size() ثابت override;
      vvp_bit4_t مقدار(unsigned idx) ثابت override;
      vvp_scalar_t scalar_مقدار(unsigned idx) ثابت override;
      تهی vec4_مقدار(vvp_vector4_t&) ثابت override;

        // Support for $countdrivers
      vvp_bit4_t driven_مقدار(unsigned idx) ثابت override;
      bool is_forced(unsigned idx) ثابت override;

    خصوصی:
      vvp_bit4_t filtered_مقدار(unsigned idx) ثابت;

    خصوصی:
      bool needs_init_;
      vvp_vector4_t bits4_; // The tracked driven value
      vvp_vector4_t force4_; // the value being forced
};

class vvp_wire_vec8 : عمومی vvp_wire_base {

    عمومی:
      explicit vvp_wire_vec8(unsigned wid);

	// The main filter behavior for this class
      prop_t filter_vec4(ثابت vvp_vector4_t&bit, vvp_vector4_t&rep,
			 unsigned base, unsigned vwid) override;
      prop_t filter_vec8(ثابت vvp_vector8_t&val, vvp_vector8_t&rep,
			 unsigned base, unsigned vwid) override;

	// island ports use this method to filter arbitrary values
	// through the force filter.
      prop_t filter_درون_داد_vec8(ثابت vvp_vector8_t&val, vvp_vector8_t&rep) ثابت;


	// Abstract methods from vvp_vpi_callback
      تهی get_مقدار(ساختار t_vpi_مقدار*مقدار) override;
	// Abstract methods from vvp_net_fit_t
      unsigned filter_size() ثابت override;
      تهی force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&mask) override;
      تهی force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&mask) override;
      تهی force_fil_real(دوگانه val, ثابت vvp_vector2_t&mask) override;
      تهی release(vvp_net_ptr_t ptr, bool net_flag) override;
      تهی release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      unsigned مقدار_size() ثابت override;
      vvp_bit4_t مقدار(unsigned idx) ثابت override;
      vvp_scalar_t scalar_مقدار(unsigned idx) ثابت override;
      تهی vec4_مقدار(vvp_vector4_t&) ثابت override;
	// This is new to vvp_wire_vec8
      vvp_vector8_t vec8_مقدار() ثابت;

        // Support for $countdrivers
      vvp_bit4_t driven_مقدار(unsigned idx) ثابت override;
      bool is_forced(unsigned idx) ثابت override;

    خصوصی:
      vvp_scalar_t filtered_مقدار(unsigned idx) ثابت;

    خصوصی:
      bool needs_init_;
      vvp_vector8_t bits8_;
      vvp_vector8_t force8_; // the value being forced
};

class vvp_wire_real : عمومی vvp_wire_base {

    عمومی:
      explicit vvp_wire_real(تهی);

	// The main filter behavior for this class
      prop_t filter_real(دوگانه&bit) override;

	// Abstract methods from vvp_vpi_callback
      تهی get_مقدار(ساختار t_vpi_مقدار*مقدار) override;
	// Abstract methods from vvp_net_fit_t
      unsigned filter_size() ثابت override;
      تهی force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&mask) override;
      تهی force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&mask) override;
      تهی force_fil_real(دوگانه val, ثابت vvp_vector2_t&mask) override;
      تهی release(vvp_net_ptr_t ptr, bool net_flag) override;
      تهی release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      unsigned مقدار_size() ثابت override;
      vvp_bit4_t مقدار(unsigned idx) ثابت override;
      vvp_scalar_t scalar_مقدار(unsigned idx) ثابت override;
      تهی vec4_مقدار(vvp_vector4_t&) ثابت override;
      دوگانه real_مقدار() ثابت override;

      تهی get_signal_مقدار(ساختار t_vpi_مقدار*vp) override;

    خصوصی:
      دوگانه bit_;
      دوگانه force_;
      bool needs_init_ = true;
};

#اگر 0
class vvp_wire_string : عمومی vvp_wire_base {

    عمومی:
      explicit vvp_wire_string(تهی);

	// Abstract methods from vvp_vpi_callback
      تهی get_مقدار(ساختار t_vpi_مقدار*مقدار);
	// Abstract methods from vvp_net_fil_t
      unsigned filter_size() ثابت;
      تهی force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&mask);
      تهی force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&mask);
      تهی force_fil_real(دوگانه val, ثابت vvp_vector2_t&mask);
      تهی release(vvp_net_ptr_t ptr, bool net_flag);
      تهی release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag);

	// Implementation of vvp_signal_value methods
      unsigned مقدار_size() ثابت;
      vvp_bit4_t مقدار(unsigned idx) ثابت;
      vvp_scalar_t scalar_مقدار(unsigned idx) ثابت;
      تهی vec4_مقدار(vvp_vector4_t&) ثابت;
      دوگانه real_مقدار() ثابت;

      تهی get_signal_مقدار(ساختار t_vpi_مقدار*vp);

    خصوصی:
      std::string مقدار_2;
};
#endif

#endif /* IVL_vvp_net_sig_H */
