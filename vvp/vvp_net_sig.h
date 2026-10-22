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
#अन्यथा
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


class vvp_fun_signal_base : सार्वजनिक vvp_net_fun_t {

    सार्वजनिक:
      vvp_fun_signal_base();

      शून्य deassign();
      शून्य deassign_pv(unsigned base, unsigned wid);

    सार्वजनिक:

	/* The %cassign/link instruction needs a place to write the
	   source node of the force, so that subsequent %cassign and
	   %deassign instructions can undo the link as needed. */
      class vvp_net_t*cassign_link;

    रक्षित:
      bool continuous_assign_active_;
      vvp_vector2_t assign_mask_;

    रक्षित:
	// This is true until at least one propagation happens.
      bool needs_init_;
};

/*
 * Variables and wires can have their values accessed, so this base
 * class offers the unified concept of an accessible value.
 */
class vvp_signal_मूल्य {
    सार्वजनिक:
      आभास ~vvp_signal_मूल्य() =0;
      आभास unsigned मूल्य_size() ध्रुव =0;
      आभास vvp_bit4_t मूल्य(unsigned idx) ध्रुव =0;
      आभास vvp_scalar_t scalar_मूल्य(unsigned idx) ध्रुव =0;
      आभास शून्य vec4_मूल्य(vvp_vector4_t&) ध्रुव =0;
      आभास द्विगुण real_मूल्य() ध्रुव;

      आभास शून्य get_signal_मूल्य(संरचना t_vpi_मूल्य*vp);
};

/*
 * This abstract class is a little more specific than the signal_base
 * class, in that it adds vector access methods.
 */
class vvp_fun_signal_vec : सार्वजनिक vvp_fun_signal_base {
    सार्वजनिक:
      आभास ध्रुव vvp_vector4_t& vec4_unfiltered_मूल्य() ध्रुव =0;
};

class automatic_signal_base : सार्वजनिक vvp_signal_मूल्य, सार्वजनिक vvp_net_fil_t {

    सार्वजनिक:
	// Automatic variables cannot be forced or released. Provide
	// stubs that assert.
      आभास शून्य release(vvp_net_ptr_t ptr, bool net_flag) override;
      आभास शून्य release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

      आभास unsigned filter_size() ध्रुव override;
      आभास शून्य force_fil_vec4(ध्रुव vvp_vector4_t&val, ध्रुव vvp_vector2_t&mask) override;
      आभास शून्य force_fil_vec8(ध्रुव vvp_vector8_t&val, ध्रुव vvp_vector2_t&mask) override;
      आभास शून्य force_fil_real(द्विगुण val, ध्रुव vvp_vector2_t&mask) override;
      आभास शून्य get_मूल्य(संरचना t_vpi_मूल्य*मूल्य) override;
};

/*
 * Statically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_sa : सार्वजनिक vvp_fun_signal_vec {

    सार्वजनिक:
      explicit vvp_fun_signal4_sa(unsigned wid, vvp_bit4_t init=BIT4_X);

      शून्य recv_vec4(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;
      शून्य recv_vec8(vvp_net_ptr_t port, ध्रुव vvp_vector8_t&bit) override;

	// Part select variants of above
      शून्य recv_vec4_pv(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t) override;
      शून्य recv_vec8_pv(vvp_net_ptr_t port, ध्रुव vvp_vector8_t&bit,
			unsigned base, unsigned vwid) override;

	// Get information about the vector value.
      ध्रुव vvp_vector4_t& vec4_unfiltered_मूल्य() ध्रुव override;

    गुप्त:
      vvp_vector4_t bits4_;
};

/*
 * Automatically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_aa : सार्वजनिक vvp_fun_signal_vec, सार्वजनिक automatic_signal_base, सार्वजनिक automatic_hooks_s {

    सार्वजनिक:
      explicit vvp_fun_signal4_aa(unsigned wid, vvp_bit4_t init=BIT4_X);
      ~vvp_fun_signal4_aa() override;

      शून्य alloc_instance(vvp_context_t context) override;
      शून्य reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      शून्य free_instance(vvp_context_t context) override;
#endif

      शून्य recv_vec4(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t context) override;

	// Part select variants of above
      शून्य recv_vec4_pv(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t) override;

	// Get information about the vector value.
      unsigned   मूल्य_size() ध्रुव override;
      vvp_bit4_t मूल्य(unsigned idx) ध्रुव override;
      vvp_scalar_t scalar_मूल्य(unsigned idx) ध्रुव override;
      शून्य vec4_मूल्य(vvp_vector4_t&) ध्रुव override;
      ध्रुव vvp_vector4_t& vec4_unfiltered_मूल्य() ध्रुव override;

    सार्वजनिक: // These objects are only permallocated.
      स्थिर शून्य* operator new(std::size_t size) { प्रत्यागम vvp_net_fun_t::heap_.alloc(size); }
      स्थिर शून्य operator delete(शून्य*obj);

    गुप्त:
      unsigned context_idx_;
      unsigned size_;
      vvp_bit4_t init_;
};

class vvp_fun_signal_real : सार्वजनिक vvp_fun_signal_base {

    सार्वजनिक:
      explicit vvp_fun_signal_real() {};

	// Get information about the vector value.
      आभास द्विगुण real_unfiltered_मूल्य() ध्रुव = 0;
};

/*
 * Statically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_sa : सार्वजनिक vvp_fun_signal_real {

    सार्वजनिक:
      explicit vvp_fun_signal_real_sa();

      शून्य recv_real(vvp_net_ptr_t port, द्विगुण bit,
                     vvp_context_t) override;

	// Get information about the vector value.
      द्विगुण real_unfiltered_मूल्य() ध्रुव override;

    गुप्त:
      द्विगुण bits_;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_aa : सार्वजनिक vvp_fun_signal_real, सार्वजनिक automatic_signal_base, सार्वजनिक automatic_hooks_s {

    सार्वजनिक:
      explicit vvp_fun_signal_real_aa();
      ~vvp_fun_signal_real_aa() override;

      शून्य alloc_instance(vvp_context_t context) override;
      शून्य reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      शून्य free_instance(vvp_context_t context) override;
#endif

      शून्य recv_real(vvp_net_ptr_t port, द्विगुण bit,
                     vvp_context_t context) override;

	// Get information about the vector value.
      द्विगुण real_unfiltered_मूल्य() ध्रुव override;

	// Get information about the vector value.
      unsigned   मूल्य_size() ध्रुव override;
      vvp_bit4_t मूल्य(unsigned idx) ध्रुव override;
      vvp_scalar_t scalar_मूल्य(unsigned idx) ध्रुव override;
      शून्य vec4_मूल्य(vvp_vector4_t&) ध्रुव override;
      द्विगुण real_मूल्य() ध्रुव override;
      शून्य get_signal_मूल्य(संरचना t_vpi_मूल्य*vp) override;

    सार्वजनिक: // These objects are only permallocated.
      स्थिर शून्य* operator new(std::size_t size);
      स्थिर शून्य operator delete(शून्य*obj);

    गुप्त:
      unsigned context_idx_;
};


class vvp_fun_signal_string : सार्वजनिक vvp_fun_signal_base {

    सार्वजनिक:
      explicit vvp_fun_signal_string() {};

      आभास ध्रुव std::string& get_string() ध्रुव =0;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_string_sa : सार्वजनिक vvp_fun_signal_string {

    सार्वजनिक:
      explicit vvp_fun_signal_string_sa();

      शून्य recv_string(vvp_net_ptr_t port, ध्रुव std::string&bit,
		       vvp_context_t context) override;

      ध्रुव std::string& get_string() ध्रुव override;

    गुप्त:
      std::string मूल्य_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_string_aa : सार्वजनिक vvp_fun_signal_string, सार्वजनिक automatic_signal_base, सार्वजनिक automatic_hooks_s {

    सार्वजनिक:
      explicit vvp_fun_signal_string_aa();
      ~vvp_fun_signal_string_aa() override;

      शून्य alloc_instance(vvp_context_t context) override;
      शून्य reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      शून्य free_instance(vvp_context_t context) override;
#endif
      शून्य recv_string(vvp_net_ptr_t port, ध्रुव std::string&bit,
		       vvp_context_t context) override;

	// Get information about the vector value.
      unsigned   मूल्य_size() ध्रुव override;
      vvp_bit4_t मूल्य(unsigned idx) ध्रुव override;
      vvp_scalar_t scalar_मूल्य(unsigned idx) ध्रुव override;
      शून्य vec4_मूल्य(vvp_vector4_t&) ध्रुव override;
      द्विगुण real_मूल्य() ध्रुव override;
      ध्रुव std::string& get_string() ध्रुव override;
      शून्य get_signal_मूल्य(संरचना t_vpi_मूल्य*vp) override;

    सार्वजनिक: // These objects are only permallocated.
      स्थिर शून्य* operator new(std::size_t size);
      स्थिर शून्य operator delete(शून्य*obj);

    गुप्त:
      unsigned context_idx_;
};

class vvp_fun_signal_object : सार्वजनिक vvp_fun_signal_base {

    सार्वजनिक:
      explicit vvp_fun_signal_object(unsigned size) { size_ = size; };
      unsigned size() ध्रुव { प्रत्यागम size_; }

      आभास vvp_object_t get_object() ध्रुव =0;
    गुप्त:
      unsigned size_;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_object_sa : सार्वजनिक vvp_fun_signal_object {

    सार्वजनिक:
      explicit vvp_fun_signal_object_sa(unsigned size);

      शून्य recv_object(vvp_net_ptr_t port, vvp_object_t bit,
		    vvp_context_t context) override;

      vvp_object_t get_object() ध्रुव override;

    गुप्त:
      vvp_object_t मूल्य_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_object_aa : सार्वजनिक vvp_fun_signal_object, सार्वजनिक automatic_signal_base, सार्वजनिक automatic_hooks_s {

    सार्वजनिक:
      explicit vvp_fun_signal_object_aa(unsigned size);
      ~vvp_fun_signal_object_aa() override;

      शून्य alloc_instance(vvp_context_t context) override;
      शून्य reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      शून्य free_instance(vvp_context_t context) override;
#endif

      शून्य recv_object(vvp_net_ptr_t port, vvp_object_t bit,
		    vvp_context_t context) override;

	// Get information about the vector value.
      unsigned   मूल्य_size() ध्रुव override;
      vvp_bit4_t मूल्य(unsigned idx) ध्रुव override;
      vvp_scalar_t scalar_मूल्य(unsigned idx) ध्रुव override;
      शून्य vec4_मूल्य(vvp_vector4_t&) ध्रुव override;
	//double real_value() const;
	//void get_signal_value(struct t_vpi_value*vp);

      vvp_object_t get_object() ध्रुव override;

    सार्वजनिक: // These objects are only permallocated.
      स्थिर शून्य* operator new(std::size_t size);
      स्थिर शून्य operator delete(शून्य*obj);

    गुप्त:
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

class vvp_wire_base  : सार्वजनिक vvp_net_fil_t, सार्वजनिक vvp_signal_मूल्य {

    सार्वजनिक:
      vvp_wire_base();
      ~vvp_wire_base() override;

        // Support for $countdrivers
      आभास vvp_bit4_t driven_मूल्य(unsigned idx) ध्रुव;
      आभास bool is_forced(unsigned idx) ध्रुव;
};

class vvp_wire_vec4 : सार्वजनिक vvp_wire_base {

    सार्वजनिक:
      vvp_wire_vec4(unsigned wid, vvp_bit4_t init);

	// The main filter behavior for this class. These methods take
	// the value that the node is driven to, and applies the force
	// filters. In wires, this also saves the driven value, so
	// that when a force is released, we can revert to the driven value.
      prop_t filter_vec4(ध्रुव vvp_vector4_t&bit, vvp_vector4_t&rep,
			 unsigned base, unsigned vwid) override;
      prop_t filter_vec8(ध्रुव vvp_vector8_t&val, vvp_vector8_t&rep,
			 unsigned base, unsigned vwid) override;

	// Abstract methods from vvp_vpi_callback
      शून्य get_मूल्य(संरचना t_vpi_मूल्य*मूल्य) override;
	// Abstract methods from vvp_net_fit_t
      unsigned filter_size() ध्रुव override;
      शून्य force_fil_vec4(ध्रुव vvp_vector4_t&val, ध्रुव vvp_vector2_t&mask) override;
      शून्य force_fil_vec8(ध्रुव vvp_vector8_t&val, ध्रुव vvp_vector2_t&mask) override;
      शून्य force_fil_real(द्विगुण val, ध्रुव vvp_vector2_t&mask) override;
      शून्य release(vvp_net_ptr_t ptr, bool net_flag) override;
      शून्य release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      unsigned मूल्य_size() ध्रुव override;
      vvp_bit4_t मूल्य(unsigned idx) ध्रुव override;
      vvp_scalar_t scalar_मूल्य(unsigned idx) ध्रुव override;
      शून्य vec4_मूल्य(vvp_vector4_t&) ध्रुव override;

        // Support for $countdrivers
      vvp_bit4_t driven_मूल्य(unsigned idx) ध्रुव override;
      bool is_forced(unsigned idx) ध्रुव override;

    गुप्त:
      vvp_bit4_t filtered_मूल्य(unsigned idx) ध्रुव;

    गुप्त:
      bool needs_init_;
      vvp_vector4_t bits4_; // The tracked driven value
      vvp_vector4_t force4_; // the value being forced
};

class vvp_wire_vec8 : सार्वजनिक vvp_wire_base {

    सार्वजनिक:
      explicit vvp_wire_vec8(unsigned wid);

	// The main filter behavior for this class
      prop_t filter_vec4(ध्रुव vvp_vector4_t&bit, vvp_vector4_t&rep,
			 unsigned base, unsigned vwid) override;
      prop_t filter_vec8(ध्रुव vvp_vector8_t&val, vvp_vector8_t&rep,
			 unsigned base, unsigned vwid) override;

	// island ports use this method to filter arbitrary values
	// through the force filter.
      prop_t filter_निवेश_vec8(ध्रुव vvp_vector8_t&val, vvp_vector8_t&rep) ध्रुव;


	// Abstract methods from vvp_vpi_callback
      शून्य get_मूल्य(संरचना t_vpi_मूल्य*मूल्य) override;
	// Abstract methods from vvp_net_fit_t
      unsigned filter_size() ध्रुव override;
      शून्य force_fil_vec4(ध्रुव vvp_vector4_t&val, ध्रुव vvp_vector2_t&mask) override;
      शून्य force_fil_vec8(ध्रुव vvp_vector8_t&val, ध्रुव vvp_vector2_t&mask) override;
      शून्य force_fil_real(द्विगुण val, ध्रुव vvp_vector2_t&mask) override;
      शून्य release(vvp_net_ptr_t ptr, bool net_flag) override;
      शून्य release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      unsigned मूल्य_size() ध्रुव override;
      vvp_bit4_t मूल्य(unsigned idx) ध्रुव override;
      vvp_scalar_t scalar_मूल्य(unsigned idx) ध्रुव override;
      शून्य vec4_मूल्य(vvp_vector4_t&) ध्रुव override;
	// This is new to vvp_wire_vec8
      vvp_vector8_t vec8_मूल्य() ध्रुव;

        // Support for $countdrivers
      vvp_bit4_t driven_मूल्य(unsigned idx) ध्रुव override;
      bool is_forced(unsigned idx) ध्रुव override;

    गुप्त:
      vvp_scalar_t filtered_मूल्य(unsigned idx) ध्रुव;

    गुप्त:
      bool needs_init_;
      vvp_vector8_t bits8_;
      vvp_vector8_t force8_; // the value being forced
};

class vvp_wire_real : सार्वजनिक vvp_wire_base {

    सार्वजनिक:
      explicit vvp_wire_real(शून्य);

	// The main filter behavior for this class
      prop_t filter_real(द्विगुण&bit) override;

	// Abstract methods from vvp_vpi_callback
      शून्य get_मूल्य(संरचना t_vpi_मूल्य*मूल्य) override;
	// Abstract methods from vvp_net_fit_t
      unsigned filter_size() ध्रुव override;
      शून्य force_fil_vec4(ध्रुव vvp_vector4_t&val, ध्रुव vvp_vector2_t&mask) override;
      शून्य force_fil_vec8(ध्रुव vvp_vector8_t&val, ध्रुव vvp_vector2_t&mask) override;
      शून्य force_fil_real(द्विगुण val, ध्रुव vvp_vector2_t&mask) override;
      शून्य release(vvp_net_ptr_t ptr, bool net_flag) override;
      शून्य release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      unsigned मूल्य_size() ध्रुव override;
      vvp_bit4_t मूल्य(unsigned idx) ध्रुव override;
      vvp_scalar_t scalar_मूल्य(unsigned idx) ध्रुव override;
      शून्य vec4_मूल्य(vvp_vector4_t&) ध्रुव override;
      द्विगुण real_मूल्य() ध्रुव override;

      शून्य get_signal_मूल्य(संरचना t_vpi_मूल्य*vp) override;

    गुप्त:
      द्विगुण bit_;
      द्विगुण force_;
      bool needs_init_ = true;
};

#यदि 0
class vvp_wire_string : सार्वजनिक vvp_wire_base {

    सार्वजनिक:
      explicit vvp_wire_string(शून्य);

	// Abstract methods from vvp_vpi_callback
      शून्य get_मूल्य(संरचना t_vpi_मूल्य*मूल्य);
	// Abstract methods from vvp_net_fil_t
      unsigned filter_size() ध्रुव;
      शून्य force_fil_vec4(ध्रुव vvp_vector4_t&val, ध्रुव vvp_vector2_t&mask);
      शून्य force_fil_vec8(ध्रुव vvp_vector8_t&val, ध्रुव vvp_vector2_t&mask);
      शून्य force_fil_real(द्विगुण val, ध्रुव vvp_vector2_t&mask);
      शून्य release(vvp_net_ptr_t ptr, bool net_flag);
      शून्य release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag);

	// Implementation of vvp_signal_value methods
      unsigned मूल्य_size() ध्रुव;
      vvp_bit4_t मूल्य(unsigned idx) ध्रुव;
      vvp_scalar_t scalar_मूल्य(unsigned idx) ध्रुव;
      शून्य vec4_मूल्य(vvp_vector4_t&) ध्रुव;
      द्विगुण real_मूल्य() ध्रुव;

      शून्य get_signal_मूल्य(संरचना t_vpi_मूल्य*vp);

    गुप्त:
      std::string मूल्य_2;
};
#endif

#endif /* IVL_vvp_net_sig_H */
