#جيڪڏهن_وصف_نه_ٿيل IVL_vvp_net_sig_H
#وصف_ڪريو IVL_vvp_net_sig_H
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

#جيڪڏهن_وصف_ٿيل HAVE_IOSFWD
# include  <iosfwd>
#وإلا
# include  <iostream>
#جيڪڏهن_پورو

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


class vvp_fun_signal_بنياد : عام vvp_net_fun_t {

    عام:
      vvp_fun_signal_بنياد();

      فراغ deassign();
      فراغ deassign_pv(بنانشان بنياد, بنانشان wid);

    عام:

	/* The %cassign/link instruction needs a place to write the
	   source node of the force, so that subsequent %cassign and
	   %deassign instructions can undo the link as needed. */
      class vvp_net_t*cassign_link;

    محمي:
      bool continuous_سونپيو_سرگرم;
      vvp_vector2_t سونپيو_نقاب;

    محمي:
	// This is true until at least one propagation happens.
      bool needs_init_;
};

/*
 * Variables and wires can have their values accessed, so this base
 * class offers the unified concept of an accessible value.
 */
class vvp_signal_قدر {
    عام:
      مجازي ~vvp_signal_قدر() =0;
      مجازي بنانشان قدر_سائيز() ثابت =0;
      مجازي vvp_bit4_t قدر(بنانشان idx) ثابت =0;
      مجازي vvp_scalar_t scalar_قدر(بنانشان idx) ثابت =0;
      مجازي فراغ vec4_قدر(vvp_vector4_t&) ثابت =0;
      مجازي مزدوج real_قدر() ثابت;

      مجازي فراغ get_signal_قدر(بنية t_vpi_قدر*vp);
};

/*
 * This abstract class is a little more specific than the signal_base
 * class, in that it adds vector access methods.
 */
class vvp_fun_signal_vec : عام vvp_fun_signal_بنياد {
    عام:
      مجازي ثابت vvp_vector4_t& vec4_unfiltered_قدر() ثابت =0;
};

class automatic_signal_بنياد : عام vvp_signal_قدر, عام vvp_net_fil_t {

    عام:
	// Automatic variables cannot be forced or released. Provide
	// stubs that assert.
      مجازي فراغ release(vvp_net_ptr_t ptr, bool net_flag) override;
      مجازي فراغ release_pv(vvp_net_ptr_t ptr, بنانشان بنياد, بنانشان wid, bool net_flag) override;

      مجازي بنانشان filter_سائيز() ثابت override;
      مجازي فراغ force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&نقاب) override;
      مجازي فراغ force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&نقاب) override;
      مجازي فراغ force_fil_real(مزدوج val, ثابت vvp_vector2_t&نقاب) override;
      مجازي فراغ get_قدر(بنية t_vpi_قدر*قدر) override;
};

/*
 * Statically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_sa : عام vvp_fun_signal_vec {

    عام:
      explicit vvp_fun_signal4_sa(بنانشان wid, vvp_bit4_t init=BIT4_X);

      فراغ recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                     vvp_context_t) override;
      فراغ recv_vec8(vvp_net_ptr_t پورٽ, ثابت vvp_vector8_t&ٻٽو_انگ) override;

	// Part select variants of above
      فراغ recv_vec4_pv(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
			بنانشان بنياد, بنانشان vwid, vvp_context_t) override;
      فراغ recv_vec8_pv(vvp_net_ptr_t پورٽ, ثابت vvp_vector8_t&ٻٽو_انگ,
			بنانشان بنياد, بنانشان vwid) override;

	// Get information about the vector value.
      ثابت vvp_vector4_t& vec4_unfiltered_قدر() ثابت override;

    خاص:
      vvp_vector4_t bits4_;
};

/*
 * Automatically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_aa : عام vvp_fun_signal_vec, عام automatic_signal_بنياد, عام automatic_hooks_s {

    عام:
      explicit vvp_fun_signal4_aa(بنانشان wid, vvp_bit4_t init=BIT4_X);
      ~vvp_fun_signal4_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#جيڪڏهن_پورو

      فراغ recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                     vvp_context_t context) override;

	// Part select variants of above
      فراغ recv_vec4_pv(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
			بنانشان بنياد, بنانشان vwid, vvp_context_t) override;

	// Get information about the vector value.
      بنانشان   قدر_سائيز() ثابت override;
      vvp_bit4_t قدر(بنانشان idx) ثابت override;
      vvp_scalar_t scalar_قدر(بنانشان idx) ثابت override;
      فراغ vec4_قدر(vvp_vector4_t&) ثابت override;
      ثابت vvp_vector4_t& vec4_unfiltered_قدر() ثابت override;

    عام: // These objects are only permallocated.
      ساكن فراغ* عامل new(std::size_t سائيز) { أرجع vvp_net_fun_t::ڍير.alloc(سائيز); }
      ساكن فراغ عامل delete(فراغ*obj);

    خاص:
      بنانشان context_idx_;
      بنانشان سائيز_2;
      vvp_bit4_t init_;
};

class vvp_fun_signal_real : عام vvp_fun_signal_بنياد {

    عام:
      explicit vvp_fun_signal_real() {};

	// Get information about the vector value.
      مجازي مزدوج real_unfiltered_قدر() ثابت = 0;
};

/*
 * Statically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_sa : عام vvp_fun_signal_real {

    عام:
      explicit vvp_fun_signal_real_sa();

      فراغ recv_real(vvp_net_ptr_t پورٽ, مزدوج ٻٽو_انگ,
                     vvp_context_t) override;

	// Get information about the vector value.
      مزدوج real_unfiltered_قدر() ثابت override;

    خاص:
      مزدوج بٽ_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_aa : عام vvp_fun_signal_real, عام automatic_signal_بنياد, عام automatic_hooks_s {

    عام:
      explicit vvp_fun_signal_real_aa();
      ~vvp_fun_signal_real_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#جيڪڏهن_پورو

      فراغ recv_real(vvp_net_ptr_t پورٽ, مزدوج ٻٽو_انگ,
                     vvp_context_t context) override;

	// Get information about the vector value.
      مزدوج real_unfiltered_قدر() ثابت override;

	// Get information about the vector value.
      بنانشان   قدر_سائيز() ثابت override;
      vvp_bit4_t قدر(بنانشان idx) ثابت override;
      vvp_scalar_t scalar_قدر(بنانشان idx) ثابت override;
      فراغ vec4_قدر(vvp_vector4_t&) ثابت override;
      مزدوج real_قدر() ثابت override;
      فراغ get_signal_قدر(بنية t_vpi_قدر*vp) override;

    عام: // These objects are only permallocated.
      ساكن فراغ* عامل new(std::size_t سائيز);
      ساكن فراغ عامل delete(فراغ*obj);

    خاص:
      بنانشان context_idx_;
};


class vvp_fun_signal_string : عام vvp_fun_signal_بنياد {

    عام:
      explicit vvp_fun_signal_string() {};

      مجازي ثابت std::string& get_string() ثابت =0;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_string_sa : عام vvp_fun_signal_string {

    عام:
      explicit vvp_fun_signal_string_sa();

      فراغ recv_string(vvp_net_ptr_t پورٽ, ثابت std::string&ٻٽو_انگ,
		       vvp_context_t context) override;

      ثابت std::string& get_string() ثابت override;

    خاص:
      std::string قدر_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_string_aa : عام vvp_fun_signal_string, عام automatic_signal_بنياد, عام automatic_hooks_s {

    عام:
      explicit vvp_fun_signal_string_aa();
      ~vvp_fun_signal_string_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#جيڪڏهن_پورو
      فراغ recv_string(vvp_net_ptr_t پورٽ, ثابت std::string&ٻٽو_انگ,
		       vvp_context_t context) override;

	// Get information about the vector value.
      بنانشان   قدر_سائيز() ثابت override;
      vvp_bit4_t قدر(بنانشان idx) ثابت override;
      vvp_scalar_t scalar_قدر(بنانشان idx) ثابت override;
      فراغ vec4_قدر(vvp_vector4_t&) ثابت override;
      مزدوج real_قدر() ثابت override;
      ثابت std::string& get_string() ثابت override;
      فراغ get_signal_قدر(بنية t_vpi_قدر*vp) override;

    عام: // These objects are only permallocated.
      ساكن فراغ* عامل new(std::size_t سائيز);
      ساكن فراغ عامل delete(فراغ*obj);

    خاص:
      بنانشان context_idx_;
};

class vvp_fun_signal_object : عام vvp_fun_signal_بنياد {

    عام:
      explicit vvp_fun_signal_object(بنانشان سائيز) { سائيز_2 = سائيز; };
      بنانشان سائيز() ثابت { أرجع سائيز_2; }

      مجازي vvp_object_t get_object() ثابت =0;
    خاص:
      بنانشان سائيز_2;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_object_sa : عام vvp_fun_signal_object {

    عام:
      explicit vvp_fun_signal_object_sa(بنانشان سائيز);

      فراغ recv_object(vvp_net_ptr_t پورٽ, vvp_object_t ٻٽو_انگ,
		    vvp_context_t context) override;

      vvp_object_t get_object() ثابت override;

    خاص:
      vvp_object_t قدر_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_object_aa : عام vvp_fun_signal_object, عام automatic_signal_بنياد, عام automatic_hooks_s {

    عام:
      explicit vvp_fun_signal_object_aa(بنانشان سائيز);
      ~vvp_fun_signal_object_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#جيڪڏهن_پورو

      فراغ recv_object(vvp_net_ptr_t پورٽ, vvp_object_t ٻٽو_انگ,
		    vvp_context_t context) override;

	// Get information about the vector value.
      بنانشان   قدر_سائيز() ثابت override;
      vvp_bit4_t قدر(بنانشان idx) ثابت override;
      vvp_scalar_t scalar_قدر(بنانشان idx) ثابت override;
      فراغ vec4_قدر(vvp_vector4_t&) ثابت override;
	//double real_value() const;
	//void get_signal_value(struct t_vpi_value*vp);

      vvp_object_t get_object() ثابت override;

    عام: // These objects are only permallocated.
      ساكن فراغ* عامل new(std::size_t سائيز);
      ساكن فراغ عامل delete(فراغ*obj);

    خاص:
      بنانشان context_idx_;
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

class vvp_wire_بنياد  : عام vvp_net_fil_t, عام vvp_signal_قدر {

    عام:
      vvp_wire_بنياد();
      ~vvp_wire_بنياد() override;

        // Support for $countdrivers
      مجازي vvp_bit4_t driven_قدر(بنانشان idx) ثابت;
      مجازي bool is_forced(بنانشان idx) ثابت;
};

class vvp_wire_vec4 : عام vvp_wire_بنياد {

    عام:
      vvp_wire_vec4(بنانشان wid, vvp_bit4_t init);

	// The main filter behavior for this class. These methods take
	// the value that the node is driven to, and applies the force
	// filters. In wires, this also saves the driven value, so
	// that when a force is released, we can revert to the driven value.
      prop_t filter_vec4(ثابت vvp_vector4_t&ٻٽو_انگ, vvp_vector4_t&rep,
			 بنانشان بنياد, بنانشان vwid) override;
      prop_t filter_vec8(ثابت vvp_vector8_t&val, vvp_vector8_t&rep,
			 بنانشان بنياد, بنانشان vwid) override;

	// Abstract methods from vvp_vpi_callback
      فراغ get_قدر(بنية t_vpi_قدر*قدر) override;
	// Abstract methods from vvp_net_fit_t
      بنانشان filter_سائيز() ثابت override;
      فراغ force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&نقاب) override;
      فراغ force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&نقاب) override;
      فراغ force_fil_real(مزدوج val, ثابت vvp_vector2_t&نقاب) override;
      فراغ release(vvp_net_ptr_t ptr, bool net_flag) override;
      فراغ release_pv(vvp_net_ptr_t ptr, بنانشان بنياد, بنانشان wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      بنانشان قدر_سائيز() ثابت override;
      vvp_bit4_t قدر(بنانشان idx) ثابت override;
      vvp_scalar_t scalar_قدر(بنانشان idx) ثابت override;
      فراغ vec4_قدر(vvp_vector4_t&) ثابت override;

        // Support for $countdrivers
      vvp_bit4_t driven_قدر(بنانشان idx) ثابت override;
      bool is_forced(بنانشان idx) ثابت override;

    خاص:
      vvp_bit4_t filtered_قدر(بنانشان idx) ثابت;

    خاص:
      bool needs_init_;
      vvp_vector4_t bits4_; // The tracked driven value
      vvp_vector4_t force4_; // the value being forced
};

class vvp_wire_vec8 : عام vvp_wire_بنياد {

    عام:
      explicit vvp_wire_vec8(بنانشان wid);

	// The main filter behavior for this class
      prop_t filter_vec4(ثابت vvp_vector4_t&ٻٽو_انگ, vvp_vector4_t&rep,
			 بنانشان بنياد, بنانشان vwid) override;
      prop_t filter_vec8(ثابت vvp_vector8_t&val, vvp_vector8_t&rep,
			 بنانشان بنياد, بنانشان vwid) override;

	// island ports use this method to filter arbitrary values
	// through the force filter.
      prop_t filter_داخلا_vec8(ثابت vvp_vector8_t&val, vvp_vector8_t&rep) ثابت;


	// Abstract methods from vvp_vpi_callback
      فراغ get_قدر(بنية t_vpi_قدر*قدر) override;
	// Abstract methods from vvp_net_fit_t
      بنانشان filter_سائيز() ثابت override;
      فراغ force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&نقاب) override;
      فراغ force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&نقاب) override;
      فراغ force_fil_real(مزدوج val, ثابت vvp_vector2_t&نقاب) override;
      فراغ release(vvp_net_ptr_t ptr, bool net_flag) override;
      فراغ release_pv(vvp_net_ptr_t ptr, بنانشان بنياد, بنانشان wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      بنانشان قدر_سائيز() ثابت override;
      vvp_bit4_t قدر(بنانشان idx) ثابت override;
      vvp_scalar_t scalar_قدر(بنانشان idx) ثابت override;
      فراغ vec4_قدر(vvp_vector4_t&) ثابت override;
	// This is new to vvp_wire_vec8
      vvp_vector8_t vec8_قدر() ثابت;

        // Support for $countdrivers
      vvp_bit4_t driven_قدر(بنانشان idx) ثابت override;
      bool is_forced(بنانشان idx) ثابت override;

    خاص:
      vvp_scalar_t filtered_قدر(بنانشان idx) ثابت;

    خاص:
      bool needs_init_;
      vvp_vector8_t bits8_;
      vvp_vector8_t force8_; // the value being forced
};

class vvp_wire_real : عام vvp_wire_بنياد {

    عام:
      explicit vvp_wire_real(فراغ);

	// The main filter behavior for this class
      prop_t filter_real(مزدوج&ٻٽو_انگ) override;

	// Abstract methods from vvp_vpi_callback
      فراغ get_قدر(بنية t_vpi_قدر*قدر) override;
	// Abstract methods from vvp_net_fit_t
      بنانشان filter_سائيز() ثابت override;
      فراغ force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&نقاب) override;
      فراغ force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&نقاب) override;
      فراغ force_fil_real(مزدوج val, ثابت vvp_vector2_t&نقاب) override;
      فراغ release(vvp_net_ptr_t ptr, bool net_flag) override;
      فراغ release_pv(vvp_net_ptr_t ptr, بنانشان بنياد, بنانشان wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      بنانشان قدر_سائيز() ثابت override;
      vvp_bit4_t قدر(بنانشان idx) ثابت override;
      vvp_scalar_t scalar_قدر(بنانشان idx) ثابت override;
      فراغ vec4_قدر(vvp_vector4_t&) ثابت override;
      مزدوج real_قدر() ثابت override;

      فراغ get_signal_قدر(بنية t_vpi_قدر*vp) override;

    خاص:
      مزدوج ٻٽو_انگ_2;
      مزدوج force_;
      bool needs_init_ = true;
};

#إذا 0
class vvp_wire_string : عام vvp_wire_بنياد {

    عام:
      explicit vvp_wire_string(فراغ);

	// Abstract methods from vvp_vpi_callback
      فراغ get_قدر(بنية t_vpi_قدر*قدر);
	// Abstract methods from vvp_net_fil_t
      بنانشان filter_سائيز() ثابت;
      فراغ force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&نقاب);
      فراغ force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&نقاب);
      فراغ force_fil_real(مزدوج val, ثابت vvp_vector2_t&نقاب);
      فراغ release(vvp_net_ptr_t ptr, bool net_flag);
      فراغ release_pv(vvp_net_ptr_t ptr, بنانشان بنياد, بنانشان wid, bool net_flag);

	// Implementation of vvp_signal_value methods
      بنانشان قدر_سائيز() ثابت;
      vvp_bit4_t قدر(بنانشان idx) ثابت;
      vvp_scalar_t scalar_قدر(بنانشان idx) ثابت;
      فراغ vec4_قدر(vvp_vector4_t&) ثابت;
      مزدوج real_قدر() ثابت;

      فراغ get_signal_قدر(بنية t_vpi_قدر*vp);

    خاص:
      std::string قدر_2;
};
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* IVL_vvp_net_sig_H */
