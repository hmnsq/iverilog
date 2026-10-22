#اگر_تعریف_نشده IVL_vvp_net_sig_H
#تعریف_کن IVL_vvp_net_sig_H
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

#اگر_تعریف_شده HAVE_IOSFWD
# include  <iosfwd>
#وإلا
# include  <iostream>
#پایان_اگر

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


class vvp_fun_signal_پایه : عام vvp_net_fun_t {

    عام:
      vvp_fun_signal_پایه();

      فراغ deassign();
      فراغ deassign_pv(بیعلامت پایه, بیعلامت wid);

    عام:

	/* The %cassign/link instruction needs a place to write the
	   source node of the force, so that subsequent %cassign and
	   %deassign instructions can undo the link as needed. */
      class vvp_net_t*cassign_link;

    محمي:
      bool continuous_تخصیص_بده_فعال;
      vvp_vector2_t تخصیص_بده_ماسک;

    محمي:
	// This is true until at least one propagation happens.
      bool needs_init_;
};

/*
 * Variables and wires can have their values accessed, so this base
 * class offers the unified concept of an accessible value.
 */
class vvp_signal_مقدار {
    عام:
      مجازی ~vvp_signal_مقدار() =0;
      مجازی بیعلامت مقدار_اندازه() ثابت =0;
      مجازی vvp_bit4_t مقدار(بیعلامت idx) ثابت =0;
      مجازی vvp_scalar_t scalar_مقدار(بیعلامت idx) ثابت =0;
      مجازی فراغ vec4_مقدار(vvp_vector4_t&) ثابت =0;
      مجازی مزدوج real_مقدار() ثابت;

      مجازی فراغ get_signal_مقدار(بنية t_vpi_مقدار*vp);
};

/*
 * This abstract class is a little more specific than the signal_base
 * class, in that it adds vector access methods.
 */
class vvp_fun_signal_vec : عام vvp_fun_signal_پایه {
    عام:
      مجازی ثابت vvp_vector4_t& vec4_unfiltered_مقدار() ثابت =0;
};

class automatic_signal_پایه : عام vvp_signal_مقدار, عام vvp_net_fil_t {

    عام:
	// Automatic variables cannot be forced or released. Provide
	// stubs that assert.
      مجازی فراغ release(vvp_net_ptr_t ptr, bool net_flag) override;
      مجازی فراغ release_pv(vvp_net_ptr_t ptr, بیعلامت پایه, بیعلامت wid, bool net_flag) override;

      مجازی بیعلامت filter_اندازه() ثابت override;
      مجازی فراغ force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&ماسک) override;
      مجازی فراغ force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&ماسک) override;
      مجازی فراغ force_fil_real(مزدوج val, ثابت vvp_vector2_t&ماسک) override;
      مجازی فراغ get_مقدار(بنية t_vpi_مقدار*مقدار) override;
};

/*
 * Statically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_sa : عام vvp_fun_signal_vec {

    عام:
      explicit vvp_fun_signal4_sa(بیعلامت wid, vvp_bit4_t init=BIT4_X);

      فراغ recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                     vvp_context_t) override;
      فراغ recv_vec8(vvp_net_ptr_t پورت, ثابت vvp_vector8_t&بیت) override;

	// Part select variants of above
      فراغ recv_vec4_pv(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
			بیعلامت پایه, بیعلامت vwid, vvp_context_t) override;
      فراغ recv_vec8_pv(vvp_net_ptr_t پورت, ثابت vvp_vector8_t&بیت,
			بیعلامت پایه, بیعلامت vwid) override;

	// Get information about the vector value.
      ثابت vvp_vector4_t& vec4_unfiltered_مقدار() ثابت override;

    خاص:
      vvp_vector4_t bits4_;
};

/*
 * Automatically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_aa : عام vvp_fun_signal_vec, عام automatic_signal_پایه, عام automatic_hooks_s {

    عام:
      explicit vvp_fun_signal4_aa(بیعلامت wid, vvp_bit4_t init=BIT4_X);
      ~vvp_fun_signal4_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#اگر_تعریف_شده CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#پایان_اگر

      فراغ recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                     vvp_context_t context) override;

	// Part select variants of above
      فراغ recv_vec4_pv(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
			بیعلامت پایه, بیعلامت vwid, vvp_context_t) override;

	// Get information about the vector value.
      بیعلامت   مقدار_اندازه() ثابت override;
      vvp_bit4_t مقدار(بیعلامت idx) ثابت override;
      vvp_scalar_t scalar_مقدار(بیعلامت idx) ثابت override;
      فراغ vec4_مقدار(vvp_vector4_t&) ثابت override;
      ثابت vvp_vector4_t& vec4_unfiltered_مقدار() ثابت override;

    عام: // These objects are only permallocated.
      ساكن فراغ* عملگر new(std::size_t اندازه) { أرجع vvp_net_fun_t::انباشته.alloc(اندازه); }
      ساكن فراغ عملگر delete(فراغ*obj);

    خاص:
      بیعلامت context_idx_;
      بیعلامت اندازه_2;
      vvp_bit4_t init_;
};

class vvp_fun_signal_real : عام vvp_fun_signal_پایه {

    عام:
      explicit vvp_fun_signal_real() {};

	// Get information about the vector value.
      مجازی مزدوج real_unfiltered_مقدار() ثابت = 0;
};

/*
 * Statically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_sa : عام vvp_fun_signal_real {

    عام:
      explicit vvp_fun_signal_real_sa();

      فراغ recv_real(vvp_net_ptr_t پورت, مزدوج بیت,
                     vvp_context_t) override;

	// Get information about the vector value.
      مزدوج real_unfiltered_مقدار() ثابت override;

    خاص:
      مزدوج بیتها_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_aa : عام vvp_fun_signal_real, عام automatic_signal_پایه, عام automatic_hooks_s {

    عام:
      explicit vvp_fun_signal_real_aa();
      ~vvp_fun_signal_real_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#اگر_تعریف_شده CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#پایان_اگر

      فراغ recv_real(vvp_net_ptr_t پورت, مزدوج بیت,
                     vvp_context_t context) override;

	// Get information about the vector value.
      مزدوج real_unfiltered_مقدار() ثابت override;

	// Get information about the vector value.
      بیعلامت   مقدار_اندازه() ثابت override;
      vvp_bit4_t مقدار(بیعلامت idx) ثابت override;
      vvp_scalar_t scalar_مقدار(بیعلامت idx) ثابت override;
      فراغ vec4_مقدار(vvp_vector4_t&) ثابت override;
      مزدوج real_مقدار() ثابت override;
      فراغ get_signal_مقدار(بنية t_vpi_مقدار*vp) override;

    عام: // These objects are only permallocated.
      ساكن فراغ* عملگر new(std::size_t اندازه);
      ساكن فراغ عملگر delete(فراغ*obj);

    خاص:
      بیعلامت context_idx_;
};


class vvp_fun_signal_string : عام vvp_fun_signal_پایه {

    عام:
      explicit vvp_fun_signal_string() {};

      مجازی ثابت std::string& get_string() ثابت =0;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_string_sa : عام vvp_fun_signal_string {

    عام:
      explicit vvp_fun_signal_string_sa();

      فراغ recv_string(vvp_net_ptr_t پورت, ثابت std::string&بیت,
		       vvp_context_t context) override;

      ثابت std::string& get_string() ثابت override;

    خاص:
      std::string مقدار_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_string_aa : عام vvp_fun_signal_string, عام automatic_signal_پایه, عام automatic_hooks_s {

    عام:
      explicit vvp_fun_signal_string_aa();
      ~vvp_fun_signal_string_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#اگر_تعریف_شده CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#پایان_اگر
      فراغ recv_string(vvp_net_ptr_t پورت, ثابت std::string&بیت,
		       vvp_context_t context) override;

	// Get information about the vector value.
      بیعلامت   مقدار_اندازه() ثابت override;
      vvp_bit4_t مقدار(بیعلامت idx) ثابت override;
      vvp_scalar_t scalar_مقدار(بیعلامت idx) ثابت override;
      فراغ vec4_مقدار(vvp_vector4_t&) ثابت override;
      مزدوج real_مقدار() ثابت override;
      ثابت std::string& get_string() ثابت override;
      فراغ get_signal_مقدار(بنية t_vpi_مقدار*vp) override;

    عام: // These objects are only permallocated.
      ساكن فراغ* عملگر new(std::size_t اندازه);
      ساكن فراغ عملگر delete(فراغ*obj);

    خاص:
      بیعلامت context_idx_;
};

class vvp_fun_signal_object : عام vvp_fun_signal_پایه {

    عام:
      explicit vvp_fun_signal_object(بیعلامت اندازه) { اندازه_2 = اندازه; };
      بیعلامت اندازه() ثابت { أرجع اندازه_2; }

      مجازی vvp_object_t get_object() ثابت =0;
    خاص:
      بیعلامت اندازه_2;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_object_sa : عام vvp_fun_signal_object {

    عام:
      explicit vvp_fun_signal_object_sa(بیعلامت اندازه);

      فراغ recv_object(vvp_net_ptr_t پورت, vvp_object_t بیت,
		    vvp_context_t context) override;

      vvp_object_t get_object() ثابت override;

    خاص:
      vvp_object_t مقدار_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_object_aa : عام vvp_fun_signal_object, عام automatic_signal_پایه, عام automatic_hooks_s {

    عام:
      explicit vvp_fun_signal_object_aa(بیعلامت اندازه);
      ~vvp_fun_signal_object_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#اگر_تعریف_شده CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#پایان_اگر

      فراغ recv_object(vvp_net_ptr_t پورت, vvp_object_t بیت,
		    vvp_context_t context) override;

	// Get information about the vector value.
      بیعلامت   مقدار_اندازه() ثابت override;
      vvp_bit4_t مقدار(بیعلامت idx) ثابت override;
      vvp_scalar_t scalar_مقدار(بیعلامت idx) ثابت override;
      فراغ vec4_مقدار(vvp_vector4_t&) ثابت override;
	//double real_value() const;
	//void get_signal_value(struct t_vpi_value*vp);

      vvp_object_t get_object() ثابت override;

    عام: // These objects are only permallocated.
      ساكن فراغ* عملگر new(std::size_t اندازه);
      ساكن فراغ عملگر delete(فراغ*obj);

    خاص:
      بیعلامت context_idx_;
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

class vvp_wire_پایه  : عام vvp_net_fil_t, عام vvp_signal_مقدار {

    عام:
      vvp_wire_پایه();
      ~vvp_wire_پایه() override;

        // Support for $countdrivers
      مجازی vvp_bit4_t driven_مقدار(بیعلامت idx) ثابت;
      مجازی bool is_forced(بیعلامت idx) ثابت;
};

class vvp_wire_vec4 : عام vvp_wire_پایه {

    عام:
      vvp_wire_vec4(بیعلامت wid, vvp_bit4_t init);

	// The main filter behavior for this class. These methods take
	// the value that the node is driven to, and applies the force
	// filters. In wires, this also saves the driven value, so
	// that when a force is released, we can revert to the driven value.
      prop_t filter_vec4(ثابت vvp_vector4_t&بیت, vvp_vector4_t&rep,
			 بیعلامت پایه, بیعلامت vwid) override;
      prop_t filter_vec8(ثابت vvp_vector8_t&val, vvp_vector8_t&rep,
			 بیعلامت پایه, بیعلامت vwid) override;

	// Abstract methods from vvp_vpi_callback
      فراغ get_مقدار(بنية t_vpi_مقدار*مقدار) override;
	// Abstract methods from vvp_net_fit_t
      بیعلامت filter_اندازه() ثابت override;
      فراغ force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&ماسک) override;
      فراغ force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&ماسک) override;
      فراغ force_fil_real(مزدوج val, ثابت vvp_vector2_t&ماسک) override;
      فراغ release(vvp_net_ptr_t ptr, bool net_flag) override;
      فراغ release_pv(vvp_net_ptr_t ptr, بیعلامت پایه, بیعلامت wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      بیعلامت مقدار_اندازه() ثابت override;
      vvp_bit4_t مقدار(بیعلامت idx) ثابت override;
      vvp_scalar_t scalar_مقدار(بیعلامت idx) ثابت override;
      فراغ vec4_مقدار(vvp_vector4_t&) ثابت override;

        // Support for $countdrivers
      vvp_bit4_t driven_مقدار(بیعلامت idx) ثابت override;
      bool is_forced(بیعلامت idx) ثابت override;

    خاص:
      vvp_bit4_t filtered_مقدار(بیعلامت idx) ثابت;

    خاص:
      bool needs_init_;
      vvp_vector4_t bits4_; // The tracked driven value
      vvp_vector4_t force4_; // the value being forced
};

class vvp_wire_vec8 : عام vvp_wire_پایه {

    عام:
      explicit vvp_wire_vec8(بیعلامت wid);

	// The main filter behavior for this class
      prop_t filter_vec4(ثابت vvp_vector4_t&بیت, vvp_vector4_t&rep,
			 بیعلامت پایه, بیعلامت vwid) override;
      prop_t filter_vec8(ثابت vvp_vector8_t&val, vvp_vector8_t&rep,
			 بیعلامت پایه, بیعلامت vwid) override;

	// island ports use this method to filter arbitrary values
	// through the force filter.
      prop_t filter_درون_داد_vec8(ثابت vvp_vector8_t&val, vvp_vector8_t&rep) ثابت;


	// Abstract methods from vvp_vpi_callback
      فراغ get_مقدار(بنية t_vpi_مقدار*مقدار) override;
	// Abstract methods from vvp_net_fit_t
      بیعلامت filter_اندازه() ثابت override;
      فراغ force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&ماسک) override;
      فراغ force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&ماسک) override;
      فراغ force_fil_real(مزدوج val, ثابت vvp_vector2_t&ماسک) override;
      فراغ release(vvp_net_ptr_t ptr, bool net_flag) override;
      فراغ release_pv(vvp_net_ptr_t ptr, بیعلامت پایه, بیعلامت wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      بیعلامت مقدار_اندازه() ثابت override;
      vvp_bit4_t مقدار(بیعلامت idx) ثابت override;
      vvp_scalar_t scalar_مقدار(بیعلامت idx) ثابت override;
      فراغ vec4_مقدار(vvp_vector4_t&) ثابت override;
	// This is new to vvp_wire_vec8
      vvp_vector8_t vec8_مقدار() ثابت;

        // Support for $countdrivers
      vvp_bit4_t driven_مقدار(بیعلامت idx) ثابت override;
      bool is_forced(بیعلامت idx) ثابت override;

    خاص:
      vvp_scalar_t filtered_مقدار(بیعلامت idx) ثابت;

    خاص:
      bool needs_init_;
      vvp_vector8_t bits8_;
      vvp_vector8_t force8_; // the value being forced
};

class vvp_wire_real : عام vvp_wire_پایه {

    عام:
      explicit vvp_wire_real(فراغ);

	// The main filter behavior for this class
      prop_t filter_real(مزدوج&بیت) override;

	// Abstract methods from vvp_vpi_callback
      فراغ get_مقدار(بنية t_vpi_مقدار*مقدار) override;
	// Abstract methods from vvp_net_fit_t
      بیعلامت filter_اندازه() ثابت override;
      فراغ force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&ماسک) override;
      فراغ force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&ماسک) override;
      فراغ force_fil_real(مزدوج val, ثابت vvp_vector2_t&ماسک) override;
      فراغ release(vvp_net_ptr_t ptr, bool net_flag) override;
      فراغ release_pv(vvp_net_ptr_t ptr, بیعلامت پایه, بیعلامت wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      بیعلامت مقدار_اندازه() ثابت override;
      vvp_bit4_t مقدار(بیعلامت idx) ثابت override;
      vvp_scalar_t scalar_مقدار(بیعلامت idx) ثابت override;
      فراغ vec4_مقدار(vvp_vector4_t&) ثابت override;
      مزدوج real_مقدار() ثابت override;

      فراغ get_signal_مقدار(بنية t_vpi_مقدار*vp) override;

    خاص:
      مزدوج بیت_2;
      مزدوج force_;
      bool needs_init_ = true;
};

#إذا 0
class vvp_wire_string : عام vvp_wire_پایه {

    عام:
      explicit vvp_wire_string(فراغ);

	// Abstract methods from vvp_vpi_callback
      فراغ get_مقدار(بنية t_vpi_مقدار*مقدار);
	// Abstract methods from vvp_net_fil_t
      بیعلامت filter_اندازه() ثابت;
      فراغ force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&ماسک);
      فراغ force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&ماسک);
      فراغ force_fil_real(مزدوج val, ثابت vvp_vector2_t&ماسک);
      فراغ release(vvp_net_ptr_t ptr, bool net_flag);
      فراغ release_pv(vvp_net_ptr_t ptr, بیعلامت پایه, بیعلامت wid, bool net_flag);

	// Implementation of vvp_signal_value methods
      بیعلامت مقدار_اندازه() ثابت;
      vvp_bit4_t مقدار(بیعلامت idx) ثابت;
      vvp_scalar_t scalar_مقدار(بیعلامت idx) ثابت;
      فراغ vec4_مقدار(vvp_vector4_t&) ثابت;
      مزدوج real_مقدار() ثابت;

      فراغ get_signal_مقدار(بنية t_vpi_مقدار*vp);

    خاص:
      std::string مقدار_2;
};
#پایان_اگر

#پایان_اگر /* IVL_vvp_net_sig_H */
