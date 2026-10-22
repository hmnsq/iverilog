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
#وإلا
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


class vvp_fun_signal_base : عام vvp_net_fun_t {

    عام:
      vvp_fun_signal_base();

      فراغ deassign();
      فراغ deassign_pv(unsigned base, unsigned wid);

    عام:

	/* The %cassign/link instruction needs a place to write the
	   source node of the force, so that subsequent %cassign and
	   %deassign instructions can undo the link as needed. */
      class vvp_net_t*cassign_link;

    محمي:
      bool continuous_assign_active_;
      vvp_vector2_t assign_mask_;

    محمي:
	// This is true until at least one propagation happens.
      bool needs_init_;
};

/*
 * Variables and wires can have their values accessed, so this base
 * class offers the unified concept of an accessible value.
 */
class vvp_signal_قيمة {
    عام:
      virtual ~vvp_signal_قيمة() =0;
      virtual unsigned قيمة_size() ثابت =0;
      virtual vvp_bit4_t قيمة(unsigned idx) ثابت =0;
      virtual vvp_scalar_t scalar_قيمة(unsigned idx) ثابت =0;
      virtual فراغ vec4_قيمة(vvp_vector4_t&) ثابت =0;
      virtual مزدوج real_قيمة() ثابت;

      virtual فراغ get_signal_قيمة(بنية t_vpi_قيمة*vp);
};

/*
 * This abstract class is a little more specific than the signal_base
 * class, in that it adds vector access methods.
 */
class vvp_fun_signal_vec : عام vvp_fun_signal_base {
    عام:
      virtual ثابت vvp_vector4_t& vec4_unfiltered_قيمة() ثابت =0;
};

class automatic_signal_base : عام vvp_signal_قيمة, عام vvp_net_fil_t {

    عام:
	// Automatic variables cannot be forced or released. Provide
	// stubs that assert.
      virtual فراغ release(vvp_net_ptr_t ptr, bool net_flag) override;
      virtual فراغ release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

      virtual unsigned filter_size() ثابت override;
      virtual فراغ force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&mask) override;
      virtual فراغ force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&mask) override;
      virtual فراغ force_fil_real(مزدوج val, ثابت vvp_vector2_t&mask) override;
      virtual فراغ get_قيمة(بنية t_vpi_قيمة*قيمة) override;
};

/*
 * Statically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_sa : عام vvp_fun_signal_vec {

    عام:
      explicit vvp_fun_signal4_sa(unsigned wid, vvp_bit4_t init=BIT4_X);

      فراغ recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                     vvp_context_t) override;
      فراغ recv_vec8(vvp_net_ptr_t port, ثابت vvp_vector8_t&bit) override;

	// Part select variants of above
      فراغ recv_vec4_pv(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t) override;
      فراغ recv_vec8_pv(vvp_net_ptr_t port, ثابت vvp_vector8_t&bit,
			unsigned base, unsigned vwid) override;

	// Get information about the vector value.
      ثابت vvp_vector4_t& vec4_unfiltered_قيمة() ثابت override;

    خاص:
      vvp_vector4_t bits4_;
};

/*
 * Automatically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_aa : عام vvp_fun_signal_vec, عام automatic_signal_base, عام automatic_hooks_s {

    عام:
      explicit vvp_fun_signal4_aa(unsigned wid, vvp_bit4_t init=BIT4_X);
      ~vvp_fun_signal4_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#endif

      فراغ recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                     vvp_context_t context) override;

	// Part select variants of above
      فراغ recv_vec4_pv(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t) override;

	// Get information about the vector value.
      unsigned   قيمة_size() ثابت override;
      vvp_bit4_t قيمة(unsigned idx) ثابت override;
      vvp_scalar_t scalar_قيمة(unsigned idx) ثابت override;
      فراغ vec4_قيمة(vvp_vector4_t&) ثابت override;
      ثابت vvp_vector4_t& vec4_unfiltered_قيمة() ثابت override;

    عام: // These objects are only permallocated.
      ساكن فراغ* operator new(std::size_t size) { أرجع vvp_net_fun_t::heap_.alloc(size); }
      ساكن فراغ operator delete(فراغ*obj);

    خاص:
      unsigned context_idx_;
      unsigned size_;
      vvp_bit4_t init_;
};

class vvp_fun_signal_real : عام vvp_fun_signal_base {

    عام:
      explicit vvp_fun_signal_real() {};

	// Get information about the vector value.
      virtual مزدوج real_unfiltered_قيمة() ثابت = 0;
};

/*
 * Statically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_sa : عام vvp_fun_signal_real {

    عام:
      explicit vvp_fun_signal_real_sa();

      فراغ recv_real(vvp_net_ptr_t port, مزدوج bit,
                     vvp_context_t) override;

	// Get information about the vector value.
      مزدوج real_unfiltered_قيمة() ثابت override;

    خاص:
      مزدوج bits_;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_aa : عام vvp_fun_signal_real, عام automatic_signal_base, عام automatic_hooks_s {

    عام:
      explicit vvp_fun_signal_real_aa();
      ~vvp_fun_signal_real_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#endif

      فراغ recv_real(vvp_net_ptr_t port, مزدوج bit,
                     vvp_context_t context) override;

	// Get information about the vector value.
      مزدوج real_unfiltered_قيمة() ثابت override;

	// Get information about the vector value.
      unsigned   قيمة_size() ثابت override;
      vvp_bit4_t قيمة(unsigned idx) ثابت override;
      vvp_scalar_t scalar_قيمة(unsigned idx) ثابت override;
      فراغ vec4_قيمة(vvp_vector4_t&) ثابت override;
      مزدوج real_قيمة() ثابت override;
      فراغ get_signal_قيمة(بنية t_vpi_قيمة*vp) override;

    عام: // These objects are only permallocated.
      ساكن فراغ* operator new(std::size_t size);
      ساكن فراغ operator delete(فراغ*obj);

    خاص:
      unsigned context_idx_;
};


class vvp_fun_signal_string : عام vvp_fun_signal_base {

    عام:
      explicit vvp_fun_signal_string() {};

      virtual ثابت std::string& get_string() ثابت =0;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_string_sa : عام vvp_fun_signal_string {

    عام:
      explicit vvp_fun_signal_string_sa();

      فراغ recv_string(vvp_net_ptr_t port, ثابت std::string&bit,
		       vvp_context_t context) override;

      ثابت std::string& get_string() ثابت override;

    خاص:
      std::string قيمة_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_string_aa : عام vvp_fun_signal_string, عام automatic_signal_base, عام automatic_hooks_s {

    عام:
      explicit vvp_fun_signal_string_aa();
      ~vvp_fun_signal_string_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#endif
      فراغ recv_string(vvp_net_ptr_t port, ثابت std::string&bit,
		       vvp_context_t context) override;

	// Get information about the vector value.
      unsigned   قيمة_size() ثابت override;
      vvp_bit4_t قيمة(unsigned idx) ثابت override;
      vvp_scalar_t scalar_قيمة(unsigned idx) ثابت override;
      فراغ vec4_قيمة(vvp_vector4_t&) ثابت override;
      مزدوج real_قيمة() ثابت override;
      ثابت std::string& get_string() ثابت override;
      فراغ get_signal_قيمة(بنية t_vpi_قيمة*vp) override;

    عام: // These objects are only permallocated.
      ساكن فراغ* operator new(std::size_t size);
      ساكن فراغ operator delete(فراغ*obj);

    خاص:
      unsigned context_idx_;
};

class vvp_fun_signal_object : عام vvp_fun_signal_base {

    عام:
      explicit vvp_fun_signal_object(unsigned size) { size_ = size; };
      unsigned size() ثابت { أرجع size_; }

      virtual vvp_object_t get_object() ثابت =0;
    خاص:
      unsigned size_;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_object_sa : عام vvp_fun_signal_object {

    عام:
      explicit vvp_fun_signal_object_sa(unsigned size);

      فراغ recv_object(vvp_net_ptr_t port, vvp_object_t bit,
		    vvp_context_t context) override;

      vvp_object_t get_object() ثابت override;

    خاص:
      vvp_object_t قيمة_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_object_aa : عام vvp_fun_signal_object, عام automatic_signal_base, عام automatic_hooks_s {

    عام:
      explicit vvp_fun_signal_object_aa(unsigned size);
      ~vvp_fun_signal_object_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#endif

      فراغ recv_object(vvp_net_ptr_t port, vvp_object_t bit,
		    vvp_context_t context) override;

	// Get information about the vector value.
      unsigned   قيمة_size() ثابت override;
      vvp_bit4_t قيمة(unsigned idx) ثابت override;
      vvp_scalar_t scalar_قيمة(unsigned idx) ثابت override;
      فراغ vec4_قيمة(vvp_vector4_t&) ثابت override;
	//double real_value() const;
	//void get_signal_value(struct t_vpi_value*vp);

      vvp_object_t get_object() ثابت override;

    عام: // These objects are only permallocated.
      ساكن فراغ* operator new(std::size_t size);
      ساكن فراغ operator delete(فراغ*obj);

    خاص:
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

class vvp_wire_base  : عام vvp_net_fil_t, عام vvp_signal_قيمة {

    عام:
      vvp_wire_base();
      ~vvp_wire_base() override;

        // Support for $countdrivers
      virtual vvp_bit4_t driven_قيمة(unsigned idx) ثابت;
      virtual bool is_forced(unsigned idx) ثابت;
};

class vvp_wire_vec4 : عام vvp_wire_base {

    عام:
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
      فراغ get_قيمة(بنية t_vpi_قيمة*قيمة) override;
	// Abstract methods from vvp_net_fit_t
      unsigned filter_size() ثابت override;
      فراغ force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&mask) override;
      فراغ force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&mask) override;
      فراغ force_fil_real(مزدوج val, ثابت vvp_vector2_t&mask) override;
      فراغ release(vvp_net_ptr_t ptr, bool net_flag) override;
      فراغ release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      unsigned قيمة_size() ثابت override;
      vvp_bit4_t قيمة(unsigned idx) ثابت override;
      vvp_scalar_t scalar_قيمة(unsigned idx) ثابت override;
      فراغ vec4_قيمة(vvp_vector4_t&) ثابت override;

        // Support for $countdrivers
      vvp_bit4_t driven_قيمة(unsigned idx) ثابت override;
      bool is_forced(unsigned idx) ثابت override;

    خاص:
      vvp_bit4_t filtered_قيمة(unsigned idx) ثابت;

    خاص:
      bool needs_init_;
      vvp_vector4_t bits4_; // The tracked driven value
      vvp_vector4_t force4_; // the value being forced
};

class vvp_wire_vec8 : عام vvp_wire_base {

    عام:
      explicit vvp_wire_vec8(unsigned wid);

	// The main filter behavior for this class
      prop_t filter_vec4(ثابت vvp_vector4_t&bit, vvp_vector4_t&rep,
			 unsigned base, unsigned vwid) override;
      prop_t filter_vec8(ثابت vvp_vector8_t&val, vvp_vector8_t&rep,
			 unsigned base, unsigned vwid) override;

	// island ports use this method to filter arbitrary values
	// through the force filter.
      prop_t filter_مدخل_vec8(ثابت vvp_vector8_t&val, vvp_vector8_t&rep) ثابت;


	// Abstract methods from vvp_vpi_callback
      فراغ get_قيمة(بنية t_vpi_قيمة*قيمة) override;
	// Abstract methods from vvp_net_fit_t
      unsigned filter_size() ثابت override;
      فراغ force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&mask) override;
      فراغ force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&mask) override;
      فراغ force_fil_real(مزدوج val, ثابت vvp_vector2_t&mask) override;
      فراغ release(vvp_net_ptr_t ptr, bool net_flag) override;
      فراغ release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      unsigned قيمة_size() ثابت override;
      vvp_bit4_t قيمة(unsigned idx) ثابت override;
      vvp_scalar_t scalar_قيمة(unsigned idx) ثابت override;
      فراغ vec4_قيمة(vvp_vector4_t&) ثابت override;
	// This is new to vvp_wire_vec8
      vvp_vector8_t vec8_قيمة() ثابت;

        // Support for $countdrivers
      vvp_bit4_t driven_قيمة(unsigned idx) ثابت override;
      bool is_forced(unsigned idx) ثابت override;

    خاص:
      vvp_scalar_t filtered_قيمة(unsigned idx) ثابت;

    خاص:
      bool needs_init_;
      vvp_vector8_t bits8_;
      vvp_vector8_t force8_; // the value being forced
};

class vvp_wire_real : عام vvp_wire_base {

    عام:
      explicit vvp_wire_real(فراغ);

	// The main filter behavior for this class
      prop_t filter_real(مزدوج&bit) override;

	// Abstract methods from vvp_vpi_callback
      فراغ get_قيمة(بنية t_vpi_قيمة*قيمة) override;
	// Abstract methods from vvp_net_fit_t
      unsigned filter_size() ثابت override;
      فراغ force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&mask) override;
      فراغ force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&mask) override;
      فراغ force_fil_real(مزدوج val, ثابت vvp_vector2_t&mask) override;
      فراغ release(vvp_net_ptr_t ptr, bool net_flag) override;
      فراغ release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      unsigned قيمة_size() ثابت override;
      vvp_bit4_t قيمة(unsigned idx) ثابت override;
      vvp_scalar_t scalar_قيمة(unsigned idx) ثابت override;
      فراغ vec4_قيمة(vvp_vector4_t&) ثابت override;
      مزدوج real_قيمة() ثابت override;

      فراغ get_signal_قيمة(بنية t_vpi_قيمة*vp) override;

    خاص:
      مزدوج bit_;
      مزدوج force_;
      bool needs_init_ = true;
};

#إذا 0
class vvp_wire_string : عام vvp_wire_base {

    عام:
      explicit vvp_wire_string(فراغ);

	// Abstract methods from vvp_vpi_callback
      فراغ get_قيمة(بنية t_vpi_قيمة*قيمة);
	// Abstract methods from vvp_net_fil_t
      unsigned filter_size() ثابت;
      فراغ force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&mask);
      فراغ force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&mask);
      فراغ force_fil_real(مزدوج val, ثابت vvp_vector2_t&mask);
      فراغ release(vvp_net_ptr_t ptr, bool net_flag);
      فراغ release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag);

	// Implementation of vvp_signal_value methods
      unsigned قيمة_size() ثابت;
      vvp_bit4_t قيمة(unsigned idx) ثابت;
      vvp_scalar_t scalar_قيمة(unsigned idx) ثابت;
      فراغ vec4_قيمة(vvp_vector4_t&) ثابت;
      مزدوج real_قيمة() ثابت;

      فراغ get_signal_قيمة(بنية t_vpi_قيمة*vp);

    خاص:
      std::string قيمة_2;
};
#endif

#endif /* IVL_vvp_net_sig_H */
